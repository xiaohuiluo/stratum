/*
 * Copyright 2018 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef STRATUM_HAL_LIB_COMMON_PHAL_INTERFACE_H_
#define STRATUM_HAL_LIB_COMMON_PHAL_INTERFACE_H_

#include <functional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "third_party/stratum/glue/status/status.h"
#include "third_party/stratum/glue/status/statusor.h"
#include "third_party/stratum/lib/channel/channel.h"
#include "third_party/stratum/public/proto/hal.grpc.pb.h"

namespace stratum {
namespace hal {

// The "PhalInterface" class implements a hercules wrapper around the
// PHAL library. It provides an abstraction layer for accessing all the
// platform peripherals except the switch ASIC. This includes fans, QSFP modules
// power units, etc. An implementation of this class is expected to be self
// contained and hide all the platform-specific details.
class PhalInterface {
 public:
  // TransceiverEvent encapsulates the data to be sent to any transceiver event
  // handlers.
  struct TransceiverEvent {
    int slot;
    int port;
    HwState state;
  };

  // This struct wraps a transceiver module insert/removal event ChannelWriter,
  // a priority, and an id. The priority is used to prioritize ChannelWriter
  // invocation whenever a transceiver module event is received.
  struct TransceiverEventWriter {
    // ChannelWriter for sending messages on transceiver events.
    std::unique_ptr<ChannelWriter<TransceiverEvent>> writer;
    int priority;  // The priority of the Writer.
    int id;        // Unique ID of the Writer.
  };

  // The TransceiverEventWriter comparator used for sorting the container
  // holding the TransceiverEventWriter instances.
  struct TransceiverEventWriterComp {
    bool operator()(const TransceiverEventWriter& a,
                    const TransceiverEventWriter& b) const {
      return a.priority > b.priority;  // high priority first
    }
  };

  // A few predefined priority values that can be used by external functions
  // when calling RegisterTransceiverEventWriter.
  static constexpr int kTransceiverEventWriterPriorityHigh = 100;
  static constexpr int kTransceiverEventWriterPriorityMed = 10;
  static constexpr int kTransceiverEventWriterPriorityLow = 1;

  virtual ~PhalInterface() {}

  // Pushes the chassis config to the class. The ChassisConfig proto includes
  // any generic platform-independent configuration info which PHAL may need.
  // Note that platform-specific configuration is internal to the implementation
  // of this class and is not pushed from outside. This function is expected to
  // perform the coldboot init sequence if PHAL is not yet initialized by the
  // time config is pushed in the coldboot mode.
  virtual ::util::Status PushChassisConfig(const ChassisConfig& config) = 0;

  // Verifies the part of config that this class cares about. This method can
  // be called at any point to verify if the ChassisConfig proto is compatible
  // with PHAL internal info (e.g. makes sure the external SingletonPort
  // messages in ChassisConfig with the same (slot, port) match what PHAL knows
  // about transceiver modules used for that (slot, port)).
  virtual ::util::Status VerifyChassisConfig(const ChassisConfig& config) = 0;

  // Fully uninitializes PHAL. Not used for warmboot shutdown. Note that there
  // is no public method to initialize the class. The initialization is done
  // internally after the class instance is created or after
  // PushChassisConfig().
  virtual ::util::Status Shutdown() = 0;

  // TODO: Add Freeze() and Unfreeze() functions to perform NSF
  // warmboot.

  // Registers a ChannelWriter to send transceiver module (QSFP) insert/removal
  // events. The ChannelWriter sends TransceiverEvent messages which each
  // contain a (slot, port, state) tuple. The priority determines the order of
  // Writes on a transceiver event, in highest-to-lowest priority number order.
  // The returned value is the ID of the Writer, which can be used to unregister
  // it in the future. Note that as soon as a ChannelWriter is registered, we
  // expect a one-time write on all registered Writers for all present
  // transceiver modules.
  virtual ::util::StatusOr<int> RegisterTransceiverEventWriter(
      std::unique_ptr<ChannelWriter<TransceiverEvent>> writer,
      int priority) = 0;

  // Unregisters a transceiver event ChannelWriter given its ID.
  virtual ::util::Status UnregisterTransceiverEventWriter(int id) = 0;

  // Gets the front panel port info by reading the transceiver info EEPROM for
  // the module inserted in the given (slot, port). This method will also
  // return the correct data if the given (slot, port) corresponds to a
  // back plane port where there is no external transceiver module. This method
  // is expected to return error if there is no module is inserted in the
  // given (slot, port) yet.
  virtual ::util::Status GetFrontPanelPortInfo(
      int slot, int port, FrontPanelPortInfo* fp_port_info) = 0;

 protected:
  // Default constructor. To be called by the Mock class instance or any
  // factory function which uses the default constructor.
  PhalInterface() {}
};

}  // namespace hal
}  // namespace stratum

#endif  // STRATUM_HAL_LIB_COMMON_PHAL_INTERFACE_H_