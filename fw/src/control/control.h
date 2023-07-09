#pragma once

#include "control/plan.h"

#pragma pack(push, 1)
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
      float target_speed;  // radians/s
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

void    control_init();
void    control_update();
uint8_t control_report(uint8_t *buffer, uint8_t len);
