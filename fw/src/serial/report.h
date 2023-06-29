#pragma once

#include <stdint.h>

#pragma pack(push, 1)
// report_t represents mouse status report.
typedef struct {
  uint8_t battery_volts;
  uint8_t mode;
  struct {
    uint16_t left    : 10;
    uint16_t center  : 10;
    uint16_t right   : 10;
    uint8_t  padding : 2;
  } sensors;
  struct {
    uint8_t onboard : 1;
    uint8_t left    : 1;
    uint8_t right   : 1;
    uint8_t ir      : 1;
    uint8_t padding : 4;
  } leds;
  struct {
    uint16_t left;
    uint16_t right;
  } encoders;
  struct {
    uint16_t power_left;
    uint16_t power_right;
    uint8_t  forward_left  : 1;
    uint8_t  forward_right : 1;
    uint8_t  padding       : 6;
  } motors;
  struct {
    float measured_left;
    float measured_right;
    float setpoint_left;
    float setpoint_right;
  } speed;
  struct {
    float measured_left;
    float measured_right;
    float setpoint_left;
    float setpoint_right;
  } position;
  struct {
    uint32_t micros;
  } rtc;
  uint8_t padding[2];
} report_t;
#pragma pack(pop)

// report is the current report.
extern report_t report;

// report_init initializes the report module.
void report_init();

// report_send sends the report.
void report_send();
