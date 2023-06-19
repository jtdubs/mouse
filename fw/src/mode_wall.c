#include "mode_wall.h"

#include "adc.h"
#include "motor.h"
#include "pin.h"

void mode_wall_enter() {
  pin_clear(LED_BUILTIN);
  pin_clear(LED_LEFT);
  pin_clear(LED_RIGHT);
  pin_set(IR_LEDS);
  set_left_motor_dir(true);
  set_left_motor_speed(0);
  set_right_motor_dir(true);
  set_right_motor_speed(0);
}

void mode_wall_tick() {
  pin_set2(LED_LEFT, sensor_left > 100);
  pin_set2(LED_BUILTIN, sensor_center > 100);
  pin_set2(LED_RIGHT, sensor_right > 100);
}
