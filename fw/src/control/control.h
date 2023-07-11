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
// - Implments a "plan" provided by the higher-level modules, for example:
//   - A "linear motion" plan to drive a certain distance at a certain speed.
//   - A "rotational motion" plan to turn by a certain number of radians.
// - Provides feedback to the higher-level modules on the progress of the plan.
//
#pragma once

#include "control/plan.h"

#pragma pack(push, 1)
// control_report_t is the report sent by the control module.
typedef struct {
  plan_t plan;
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
      uint16_t left;    // ADC reading
      uint16_t right;   // ADC reading
      uint16_t center;  // ADC reading
    } sensor_cal;
    struct {
      float start_theta;   // radians
      float target_theta;  // radians
      bool  direction;     // true = positive, false = negative
    } rotation;
    struct {
      float start_distance;   // mm
      float target_distance;  // mm
      float target_speed;     // mm/s
    } linear;
  } plan_data;
} control_report_t;
#pragma pack(pop)

// control_init initializes the control module.
void control_init();

// control_tick executes one tick of the control module.
void control_tick();

// control_report sends a control report.
uint8_t control_report(uint8_t *buffer, uint8_t len);
