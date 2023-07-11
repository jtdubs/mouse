//
// System: platform
// Module: platform
//
// Purpose:
// - Configures all the board's peripherals.
// - Owns direction interaction with the peripherals.
//
#pragma once

#include <stdbool.h>
#include <stdint.h>

#pragma pack(push, 1)
// platform_report_t represents a report of the platform state.
typedef struct {
  uint8_t battery_volts;  // 8-bit ADC value
  struct {
    uint16_t left    : 10;  // 10-bit ADC value
    uint16_t center  : 10;  // 10-bit ADC value
    uint16_t right   : 10;  // 10-bit ADC value
    uint8_t  padding : 2;
  } sensors;
  struct {
    bool    onboard : 1;
    bool    left    : 1;
    bool    right   : 1;
    bool    ir      : 1;
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
} platform_report_t;
#pragma pack(pop)

// platform_init initializes the platform system.
void platform_init();

// platform_report returns a report of the platform state.
uint8_t platform_report(uint8_t *buffer, uint8_t len);
