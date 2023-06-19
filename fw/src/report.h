#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "mode.h"
#include "usart0.h"

#pragma pack(push)
#pragma pack(1)
// report_t represents mouse status report.
typedef struct {
  uint8_t  battery_volts;
  uint8_t  mode;
  uint16_t encoder_left;
  uint16_t encoder_right;
  struct {
    uint16_t left    : 10;
    uint16_t center  : 10;
    uint16_t right   : 10;
    uint16_t padding : 2;
  } sensors;
  struct {
    uint16_t onboard : 1;
    uint16_t left    : 1;
    uint16_t right   : 1;
    uint16_t ir      : 1;
    uint16_t padding : 4;
  } leds;
  uint8_t padding[1];
} report_t;
#pragma pack(pop)

// report is the current report.
extern report_t report;

// report_init initializes the report module.
void report_init();

// report_available determines if the report is available to be written.
inline bool report_available() {
  return usart0_write_ready();
}

// report_send sends the report.
void report_send();
