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
// - Reads from most platform modules to populate the report.
//
#pragma once

#include <stdint.h>

#pragma pack(push, 1)
// report_t represents a mouse status report.
typedef struct {
  uint8_t battery_volts;
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
    int32_t left;
    int32_t right;
  } encoders;
  struct {
    int16_t left;
    int16_t right;
  } motors;
  struct {
    float measured_left;
    float measured_right;
    float setpoint_left;
    float setpoint_right;
  } speed;
  struct {
    float distance;
    float theta;
  } position;
  struct {
    uint32_t micros;
  } rtc;
} report_t;
#pragma pack(pop)

// report is the current report.
extern report_t report;

// report_init initializes the report module.
void report_init();

// report_send sends the report.
void report_send();
