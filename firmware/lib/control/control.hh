//
// System: control
// Module: control
//
// Purpose:
// - The overall control loop for the mouse.
// - Drives the platform layer to drive all the mouse I/O.
//
// Design:
// - Executed by the platform's timer.
// - Implements a "plan" provided by the higher-level modules, for example:
//   - A "linear motion" plan to drive a certain distance at a certain speed.
//   - A "rotational motion" plan to turn by a certain number of radians.
// - Provides feedback to the higher-level modules on the progress of the plan.
//
#pragma once

#include "linear.hh"
#include "plan.hh"
#include "rotational.hh"

#if not defined(__AVR__)
#include <ostream>
#endif

namespace mouse::control {

// Init initializes the control module.
void Init();

// report sends a control report.
uint8_t GetReport(uint8_t *buffer, uint8_t len);

#pragma pack(push, 1)
// control_Report is the report sent by the control module.
struct Report {
  control::plan::Plan plan;
  struct {
    float measured_left;   // RPMs
    float measured_right;  // RPMs
    float setpoint_left;   // RPMs
    float setpoint_right;  // RPMs
  } speed;
  struct {
    float distance;  // mm
    float theta;     // radians
  } position;
  union {
    struct {
      uint16_t left;     // ADC reading
      uint16_t right;    // ADC reading
      uint16_t forward;  // ADC reading
    } sensor_cal;
    rotational::State      rotation;
    control::linear::State linear;
  } plan_data;
};
#pragma pack(pop)

#if not defined(__AVR__)
[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const Report *report) {
  o << "control::Report{" << std::endl;
  o << "  speed_measured: {" << report->speed.measured_left << ", " << report->speed.measured_right << "}" << std::endl;
  o << "  speed_setpoint: {" << report->speed.setpoint_left << ", " << report->speed.setpoint_right << "}" << std::endl;
  o << "  position: {" << report->position.distance << ", " << report->position.theta << "}" << std::endl;
  o << "  plan: " << report->plan << std::endl;
  if (report->plan.type == control::plan::Type::RotationalMotion) {
    o << "  rotation: " << report->plan_data.rotation << std::endl;
  } else if (report->plan.type == control::plan::Type::LinearMotion) {
    o << "  linear: " << report->plan_data.linear << std::endl;
  } else if (report->plan.type == control::plan::Type::SensorCal) {
    o << "  sensor_cal: {";
    o << report->plan_data.sensor_cal.left << ", ";
    o << report->plan_data.sensor_cal.forward << ", ";
    o << report->plan_data.sensor_cal.right << "}" << std::endl;
  }
  o << "}";
  return o;
}

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const Report &report) {
  return o << &report;
}
#endif

}  // namespace mouse::control
