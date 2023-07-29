#pragma once

#include "firmware/platform/platform.hh"

namespace mouse::platform {

#pragma pack(push, 1)
// platform_Report represents a report of the platform state.
struct Report {
  uint8_t battery_volts;  // 8-bit ADC value
  struct {
    uint16_t left    : 10;  // 10-bit ADC value
    uint16_t right   : 10;  // 10-bit ADC value
    uint16_t forward : 10;  // 10-bit ADC value
    uint8_t  padding : 2;
  } sensors;
  struct {
    bool    left    : 1;
    bool    right   : 1;
    bool    onboard : 1;
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
};
#pragma pack(pop)

}  // namespace mouse::platform
