#include "mode_wall.h"

#include <math.h>

#include "adc.h"
#include "motor.h"
#include "pin.h"

uint16_t distance_left;
uint16_t distance_center;
uint16_t distance_right;

uint16_t calc_distance_center();

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

  distance_center = calc_distance_center();
}

uint16_t calc_distance_center() {
  if (sensor_center < 6) {
    return 770 - (sensor_center << 6);
  }
  if (sensor_center < 18) {
    return 550 - (sensor_center << 4);
  }
  if (sensor_center < 34) {
    return 290 - (sensor_center << 2);
  }
  if (sensor_center < 50) {
    return 223 - (sensor_center << 1);
  }
  if (sensor_center < 78) {
    return 173 - sensor_center;
  }
  if (sensor_center < 116) {
    return 134 - (sensor_center >> 1);
  }
  if (sensor_center < 200) {
    return 105 - (sensor_center >> 2);
  }
  if (sensor_center < 272) {
    return 80 - (sensor_center >> 3);
  }
  if (sensor_center < 512) {
    return 63 - (sensor_center >> 4);
  }
  return 47 - (sensor_center >> 5);
}
