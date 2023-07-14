#include "walls.h"

#include "control/sensor_cal.h"
#include "platform/adc.h"
#include "platform/pin.h"

bool wall_left_present;
bool wall_right_present;
bool wall_forward_present;

int16_t wall_error_left;
int16_t wall_error_right;

static bool wall_led_control;

void walls_init() {
  wall_led_control = true;
}

void walls_led_control(bool enabled) {
  wall_led_control = enabled;
}

void walls_update() {
  uint16_t left, right, front;
  adc_read(ADC_SENSOR_LEFT, &left);
  adc_read(ADC_SENSOR_RIGHT, &right);
  adc_read(ADC_SENSOR_CENTER, &front);

  wall_left_present    = (left >= (sensor_threshold_left - 80));
  wall_right_present   = (right >= (sensor_threshold_right - 80));
  wall_forward_present = (front >= (sensor_threshold_center - 60));
  wall_error_left      = left - sensor_threshold_left;
  wall_error_right     = sensor_threshold_right - right;

  if (wall_led_control) {
    pin_set2(LED_LEFT, wall_left_present);
    pin_set2(LED_RIGHT, wall_right_present);
    pin_set2(LED_BUILTIN, wall_forward_present);
  }
}
