// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// This is a .p4 file that MeterColorMapperTest uses to test transforms
// of IR::IfStatements with meter color conditions.  It tests basic supported
// conditions based on each possible color.

#include <core.p4>
#include <v1model.p4>

#include "control_stubs.p4"
#include "simple_headers.p4"

// Test for GREEN.
control meter_if_green(inout headers hdr, inout test_metadata_t meta,
                       inout standard_metadata_t standard_metadata) {
  apply {
    if (meta.enum_color == meter_color_t.COLOR_GREEN) {
      clone3(CloneType.I2E, 1024, {});
    }
  }
}

// Test for RED.
control meter_if_red(inout headers hdr, inout test_metadata_t meta,
                     inout standard_metadata_t standard_metadata) {
  apply {
    if (meta.enum_color == meter_color_t.COLOR_RED) {
      mark_to_drop();
    }
  }
}

// Test for YELLOW.
control meter_if_yellow(inout headers hdr, inout test_metadata_t meta) {
  apply {
    if (meta.enum_color == meter_color_t.COLOR_YELLOW) {
      mark_to_drop();
    }
  }
}

V1Switch(parser_stub(), meter_if_yellow(), meter_if_green(),
         meter_if_red(), compute_checksum_stub(),
         deparser_stub()) main;
