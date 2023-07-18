#pragma once

#include "report.hh"

namespace report {

// report_type_t identifies the type of a given report.
enum class Type : uint8_t {
  Platform,
  Control,
  Maze,
  Explore,
};

// report_header_t represents a mouse report header.
#pragma pack(push, 1)
typedef struct {
  Type     type;
  uint32_t rtc_micros;
} report_header_t;
#pragma pack(pop)

// report_t represents a mouse report.
#pragma pack(push, 1)
typedef struct {
  report_header_t header;
  uint8_t         data[64];  // the format of the data is defined by the report type
} report_t;
#pragma pack(pop)

// send sends the next report.
void send();

}  // namespace report
