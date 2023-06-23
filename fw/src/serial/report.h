#pragma once

#include <stdint.h>

#pragma pack(push, 1)
// report_t represents mouse status report.
typedef struct {
  uint8_t battery_volts;  // 1
  uint8_t mode;           // 2
  struct {
    uint16_t left    : 10;
    uint16_t center  : 10;
    uint16_t right   : 10;
    uint8_t  padding : 2;
  } sensors;  // 3-6
  struct {
    uint8_t onboard : 1;
    uint8_t left    : 1;
    uint8_t right   : 1;
    uint8_t ir      : 1;
    uint8_t padding : 4;
  } leds;  // 7
  struct {
    uint16_t encoder_left;
    uint16_t encoder_right;
    uint8_t  speed_left;
    uint8_t  speed_right;
    uint8_t  forward_left  : 1;
    uint8_t  forward_right : 1;
    uint8_t  padding       : 6;
  } motors;         // 14
  uint8_t padding;  // 15
} report_t;
#pragma pack(pop)

// report is the current report.
extern report_t report;

// report_init initializes the report module.
void report_init();

// report_send sends the report.
void report_send();
