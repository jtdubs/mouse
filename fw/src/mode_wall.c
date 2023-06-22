#include "mode_wall.h"

#include <math.h>

#include "adc.h"
#include "mode.h"
#include "motor.h"
#include "pin.h"

uint8_t distance_left;
uint8_t distance_center;
uint8_t distance_right;

uint8_t calc_distance_center();

void mode_wall_enter() {
  mode_enter();
  pin_set(IR_LEDS);
}

void mode_wall_tick() {
  pin_set2(LED_LEFT, sensor_left > 100);
  pin_set2(LED_BUILTIN, sensor_center > 100);
  pin_set2(LED_RIGHT, sensor_right > 100);

  distance_center = calc_distance_center();
}

uint8_t calc_distance_center() {
  uint8_t sensor = sensor_center >> 2;

  if (sensor < 4) {
    return 255 - (sensor << 4);
  }
  if (sensor < 14) {
    return 225 - (sensor << 3);
  }
  if (sensor < 20) {
    return 172 - (sensor << 2);
  }
  if (sensor < 30) {
    return 133 - (sensor << 1);
  }
  if (sensor < 46) {
    return 103 - sensor;
  }
  if (sensor < 72) {
    return 80 - (sensor >> 1);
  }
  if (sensor < 112) {
    return 62 - (sensor >> 2);
  }
  if (sensor < 176) {
    return 48 - (sensor >> 3);
  }
  if (sensor < 203) {
    return 37 - (sensor >> 4);
  }
  return 75 - (sensor >> 2);
}
