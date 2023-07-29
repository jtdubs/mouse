//
// System: platform
// Module: platform
//
// Purpose:
// - Configures all the board's peripherals.
// - Owns direction interaction with the peripherals.
//
#pragma once

#include <stdint.h>

#include "adc.hh"
#include "encoders.hh"
#include "motor.hh"
#include "pin.hh"
#include "rtc.hh"
#include "selector.hh"
#include "timer.hh"
#include "usart0.hh"

#if not defined(__AVR__)
#include <ostream>
#endif

namespace mouse::platform {

// Init initializes the platform system.
void Init();

// report returns a report of the platform state.
uint8_t GetReport(uint8_t *buffer, uint8_t len);

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

#if not defined(__AVR__)
[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const Report *report) {
  o << "platform::Report{" << std::endl;
  o << "  battery_volts: " << static_cast<int>(report->battery_volts) << std::endl;
  o << "  sensors: {";
  o << report->sensors.left << ", ";
  o << report->sensors.right << ", ";
  o << report->sensors.forward << "}" << std::endl;
  o << "  leds: {";
  o << report->leds.left << ", ";
  o << report->leds.right << ", ";
  o << report->leds.onboard << "}" << std::endl;
  o << "  ir: " << report->leds.ir << std::endl;
  o << "  encoders: {" << report->encoders.left << ", " << report->encoders.right << "}" << std::endl;
  o << "  motors: {" << report->motors.left << ", " << report->motors.right << "}" << std::endl;
  o << "}";
  return o;
}

[[maybe_unused]] static std::ostream &operator<<(std::ostream &o, const Report &report) {
  return o << &report;
}
#endif

}  // namespace mouse::platform
