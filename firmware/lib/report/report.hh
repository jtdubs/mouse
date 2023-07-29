//
// System: serial
// Module: report
//
// Purpose:
// - Defines the data structure used to send serial reports to the host.
// - Provides functions to trigger building and sending of the report.
// - Communicates with the usart0 module to send the report.
//
// Dependencies:
// - Uses the usart0 module to transmit reports.
// - Requests reports from the platform, control, and maze modules.
//
// Design:
// - Reports are of variable size, and only transmitted if needed.
// - The modules (maze, control, plaform) are consulted in order to
//   determine if a report should be sent.
// - The first module to say "yes" wins, and the report is sent.
// - The goal is to send the most "important" report, prioritizing
//   updates to the maze state, then the control state, and sending
//   a generic platform report if nothing else is happening.
//
#pragma once

#include <stdint.h>

#include "firmware/lib/control/control.hh"
#include "firmware/lib/maze/maze.hh"
#include "firmware/lib/mode/explore/explore.hh"
#include "firmware/platform/platform.hh"

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

// Report represents a mouse report.
#pragma pack(push, 1)
struct Report {
  Type     type;
  uint32_t rtc_micros;
  uint8_t  length;
  uint8_t  data[64];  // the format of the data is defined by the report type
};
#pragma pack(pop)

// Init initializes the report module.
void Init();

#if not defined(__AVR__)
[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const Type type) {
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

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const Report *report) {
  o << "report::Report{" << std::endl;
  o << "  type: " << report->type << std::endl;
  o << "  rtc_micros: " << report->rtc_micros << std::endl;
  o << "  length: " << report->rtc_micros << std::endl;
  switch (report->type) {
    case Type::Platform:
      o << "  data: " << reinterpret_cast<const platform::Report *>(report->data) << std::endl;
      break;
    case Type::Control:
      o << "  data: " << reinterpret_cast<const control::Report *>(report->data) << std::endl;
      break;
    case Type::Maze: {
      auto   updates = reinterpret_cast<maze::Update *>(const_cast<uint8_t *>(report->data));
      size_t n       = report->length / sizeof(maze::Update);
      for (size_t i = 0; i < n; i++) {
        o << "  update: " << updates[i] << std::endl;
      }
      updates++;
      break;
    }
    case Type::Explore: {
      auto   updates = reinterpret_cast<explore::DequeueUpdate *>(const_cast<uint8_t *>(report->data));
      size_t n       = report->length / sizeof(explore::DequeueUpdate);
      for (size_t i = 0; i < n; i++) {
        o << "  update: " << updates << std::endl;
      }
      break;
    }
  }
  o << "}";
  return o;
}

#endif

}  // namespace report
