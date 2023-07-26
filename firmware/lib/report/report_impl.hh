#pragma once

#include "report.hh"

#if not defined(__AVR__)
#include <ostream>
#endif

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

#if not defined(__AVR__)
std::ostream &operator<<(std::ostream &o, const Type type) {
  switch (type) {
    case Type::Platform:
      o << "Platform";
      break;
    case Type::Control:
      o << "Control";
      break;
    case Type::Maze:
      o << "Maze";
      break;
    case Type::Explore:
      o << "Explore";
      break;
  }
  return o;
}

std::ostream &operator<<(std::ostream &o, const ReportHeader *header) {
  o << "report::ReportHeader{" << std::endl;
  o << "  type: " << header->type << std::endl;
  o << "  rtc_micros: " << header->rtc_micros << std::endl;
  o << "}";
  return o;
}

std::ostream &operator<<(std::ostream &o, const Report *report) {
  o << "report::Report{" << std::endl;
  o << "  header: " << &report->header << std::endl;
  switch (report->header.type) {
    case Type::Platform:
      o << "  data: " << reinterpret_cast<const platform::Report *>(report->data) << std::endl;
      break;
    case Type::Control:
      o << "  data: " << reinterpret_cast<const control::Report *>(report->data) << std::endl;
      break;
    case Type::Maze:
      o << "  data: " << reinterpret_cast<const maze::Report *>(report->data) << std::endl;
      break;
    case Type::Explore:
      o << "  data: " << reinterpret_cast<const explore::Report *>(report->data) << std::endl;
      break;
  }
  o << "}";
  return o;
}

#endif

}  // namespace report
