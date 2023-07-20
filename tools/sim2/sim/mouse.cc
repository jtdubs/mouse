#include "mouse.hh"

#include <math.h>
#include <simavr/avr_adc.h>

namespace sim {

Mouse::Mouse()
    : avr_(nullptr),
      leds_(),
      battery_(ADC_IRQ_ADC6),
      left_sensor_("SENSOR_LEFT", ADC_IRQ_ADC2, Position(50, 40), M_PI / 3.0),
      forward_sensor_("SENSOR_FORWARD", ADC_IRQ_ADC1, Position(65, 0), 0),
      right_sensor_("SENSOR_RIGHT", ADC_IRQ_ADC0, Position(50, -40), -M_PI / 3.0),
      selector_(ADC_IRQ_ADC6),
      left_motor_(true),
      right_motor_(false) {}

void Mouse::Init(avr_t *avr) {
  avr_ = avr;
  leds_.Init(avr);
  battery_.Init(avr);
  left_sensor_.Init(avr);
  forward_sensor_.Init(avr);
  right_sensor_.Init(avr);
  selector_.Init(avr);
  left_motor_.Init(avr);
  right_motor_.Init(avr);
}

LEDs &Mouse::GetLEDs() {
  return leds_;
}

Battery &Mouse::GetBattery() {
  return battery_;
}

FunctionSelector &Mouse::GetSelector() {
  return selector_;
}

Sensor &Mouse::GetLeftSensor() {
  return left_sensor_;
}

Sensor &Mouse::GetForwardSensor() {
  return forward_sensor_;
}

Sensor &Mouse::GetRightSensor() {
  return right_sensor_;
}

Motor &Mouse::GetLeftMotor() {
  return left_motor_;
}

Motor &Mouse::GetRightMotor() {
  return right_motor_;
}

}  // namespace sim
