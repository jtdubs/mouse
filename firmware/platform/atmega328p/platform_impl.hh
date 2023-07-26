#pragma once

#include "firmware/platform/platform.hh"

#if not defined(__AVR__)
#include <ostream>
#endif

namespace platform {

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
std::ostream &operator<<(std::ostream &o, const Report *report) {
  o << "platform::Report{" << std::endl;
  o << "  battery_volts: " << report->battery_volts << std::endl;
  o << "  sensors: {" << std::endl;
  o << "    left: " << report->sensors.left << std::endl;
  o << "    right: " << report->sensors.right << std::endl;
  o << "    forward: " << report->sensors.forward << std::endl;
  o << "  }" << std::endl;
  o << "  leds: {" << std::endl;
  o << "    left: " << report->leds.left << std::endl;
  o << "    right: " << report->leds.right << std::endl;
  o << "    onboard: " << report->leds.onboard << std::endl;
  o << "    ir: " << report->leds.ir << std::endl;
  o << "  }" << std::endl;
  o << "  encoders: {" << std::endl;
  o << "    left: " << report->encoders.left << std::endl;
  o << "    right: " << report->encoders.right << std::endl;
  o << "  }" << std::endl;
  o << "  motors: {" << std::endl;
  o << "    left: " << report->motors.left << std::endl;
  o << "    right: " << report->motors.right << std::endl;
  o << "  }" << std::endl;
  o << "}";
  return o;
}
#endif

}  // namespace platform
