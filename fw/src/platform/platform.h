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
} platform_report_t;
#pragma pack(pop)

void platform_init();

uint8_t platform_report(uint8_t *buffer, uint8_t len);
