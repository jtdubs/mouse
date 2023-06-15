#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "usart0.h"

#pragma pack(push)
#pragma pack(1)
// report_t represents mouse status report.
typedef struct {
  uint8_t battery_volts;
  uint8_t function_select;
  // uint8_t sensors[6];
  uint8_t padding;
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
