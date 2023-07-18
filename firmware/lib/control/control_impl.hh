#pragma once

#include "control.hh"
#include "linear_impl.hh"
#include "plan_impl.hh"
#include "rotational_impl.hh"

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

// tick executes one tick of the control module.
void tick();

}  // namespace control
