#pragma once

#include "control.hh"
#include "linear_impl.hh"
#include "plan_impl.hh"
#include "rotational_impl.hh"

#if not defined(__AVR__)
#include <ostream>
#endif

namespace control {

#pragma pack(push, 1)
// control_Report is the report sent by the control module.
struct Report {
  plan::Plan plan;
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
    rotational::State rotation;
    linear::State     linear;
  } plan_data;
};
#pragma pack(pop)

// Tick executes one Tick of the control module.
void Tick();

#if not defined(__AVR__)
std::ostream &operator<<(std::ostream &o, const Report *report) {
  o << "control::Report{" << std::endl;
  o << "  plan: " << report->plan << std::endl;
  o << "  speed: {" << std::endl;
  o << "    measured_left: " << report->speed.measured_left << std::endl;
  o << "    measured_right: " << report->speed.measured_right << std::endl;
  o << "    setpoint_left: " << report->speed.setpoint_left << std::endl;
  o << "    setpoint_right: " << report->speed.setpoint_right << std::endl;
  o << "  }" << std::endl;
  o << "  position: {" << std::endl;
  o << "    distance: " << report->position.distance << std::endl;
  o << "    theta: " << report->position.theta << std::endl;
  o << "  }" << std::endl;
  o << "  plan_data: {" << std::endl;
  if (report->plan == plan::Plan::Rotational) {
    o << "    rotation: " << report->plan_data.rotation << std::endl;
  } else if (report->plan == plan::Plan::Linear) {
    o << "    linear: " << report->plan_data.linear << std::endl;
  } else if (report->plan == plan::Plan::SensorCal) {
    o << "    sensor_cal: {" << std::endl;
    o << "      left: " << report->plan_data.sensor_cal.left << std::endl;
    o << "      right: " << report->plan_data.sensor_cal.right << std::endl;
    o << "      forward: " << report->plan_data.sensor_cal.forward << std::endl;
    o << "    }" << std::endl;
  }
  o << "  }" << std::endl;
  o << "}";
  return o;
}
#endif

}  // namespace control
