#pragma once

#include "report.hh"

namespace report {

// Reportype_t identifies the type of a given report.
enum class Type : uint8_t {
  Platform,
  Control,
  Maze,
  Explore,
};

// ReportHeader represents a mouse report header.
#pragma pack(push, 1)
struct ReportHeader {
  Type     type;
  uint32_t rtc_micros;
};
#pragma pack(pop)

// Report represents a mouse report.
#pragma pack(push, 1)
struct Report {
  ReportHeader header;
  uint8_t      data[64];  // the format of the data is defined by the report type
};
#pragma pack(pop)

// send sends the next report.
void Send();

}  // namespace report
