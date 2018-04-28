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


#ifndef STRATUM_HAL_LIB_COMMON_SWITCH_MOCK_H_
#define STRATUM_HAL_LIB_COMMON_SWITCH_MOCK_H_

#include "third_party/stratum/hal/lib/common/switch_interface.h"
#include "testing/base/public/gmock.h"

namespace stratum {
namespace hal {

// A mock class for SwitchInterface.
class SwitchMock : public SwitchInterface {
 public:
  MOCK_METHOD1(PushChassisConfig, ::util::Status(const ChassisConfig& config));
  MOCK_METHOD1(VerifyChassisConfig,
               ::util::Status(const ChassisConfig& config));
  MOCK_METHOD2(PushForwardingPipelineConfig,
               ::util::Status(uint64 node_id,
                              const ::p4::ForwardingPipelineConfig& config));
  MOCK_METHOD2(VerifyForwardingPipelineConfig,
               ::util::Status(uint64 node_id,
                              const ::p4::ForwardingPipelineConfig& config));
  MOCK_METHOD0(Shutdown, ::util::Status());
  MOCK_METHOD0(Freeze, ::util::Status());
  MOCK_METHOD0(Unfreeze, ::util::Status());
  MOCK_METHOD2(WriteForwardingEntries,
               ::util::Status(const ::p4::WriteRequest& req,
                              std::vector<::util::Status>* results));
  MOCK_METHOD3(ReadForwardingEntries,
               ::util::Status(const ::p4::ReadRequest& req,
                              WriterInterface<::p4::ReadResponse>* writer,
                              std::vector<::util::Status>* details));
  MOCK_METHOD2(
      RegisterPacketReceiveWriter,
      ::util::Status(
          uint64 node_id,
          const std::shared_ptr<WriterInterface<::p4::PacketIn>>& writer));
  MOCK_METHOD1(UnregisterPacketReceiveWriter, ::util::Status(uint64 node_id));
  MOCK_METHOD2(TransmitPacket,
               ::util::Status(uint64 node_id, const ::p4::PacketOut& packet));
  MOCK_METHOD1(
      RegisterEventNotifyWriter,
      ::util::Status(
          const std::shared_ptr<WriterInterface<GnmiEventPtr>>& writer));
  MOCK_METHOD0(UnregisterEventNotifyWriter, ::util::Status());
  MOCK_METHOD4(RetrieveValue,
               ::util::Status(uint64 node_id, const DataRequest& requests,
                              WriterInterface<DataResponse>* writer,
                              std::vector<::util::Status>* details));
  MOCK_METHOD0(VerifyState, ::util::StatusOr<std::vector<std::string>>());
  SwitchMock() {
    ::testing::DefaultValue<::util::Status>::Set(::util::OkStatus());
  }
};

}  // namespace hal
}  // namespace stratum

#endif  // STRATUM_HAL_LIB_COMMON_SWITCH_MOCK_H_