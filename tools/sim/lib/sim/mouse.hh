#pragma once

#include <simavr/sim_avr.h>

#include "battery.hh"
#include "functionselector.hh"
#include "leds.hh"
#include "motor.hh"
#include "sensor.hh"

namespace sim {

class Mouse {
 public:
  Mouse();

  void Init(avr_t *avr);

  LEDs             &GetLEDs();
  Battery          &GetBattery();
  FunctionSelector &GetSelector();

  Sensor &GetLeftSensor();
  Sensor &GetForwardSensor();
  Sensor &GetRightSensor();

  Motor &GetLeftMotor();
  Motor &GetRightMotor();

 private:
  avr_t           *avr_;
  LEDs             leds_;
  Battery          battery_;
  Sensor           left_sensor_, forward_sensor_, right_sensor_;
  FunctionSelector selector_;
  Motor            left_motor_, right_motor_;
};

}  // namespace sim
