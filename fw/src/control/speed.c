#include "speed.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/atomic.h>

#include "control/pid.h"
#include "platform/encoders.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "platform/rtc.h"

#define MAX_ENCODER_PERIOD 9000
#define MIN_SPEED 30.0

// Motor speeds in RPM.
float speed_measured_left;
float speed_measured_right;

// Motor speed setpoints.
float speed_setpoint_left;
float speed_setpoint_right;

pi_t speed_pi_left;
pi_t speed_pi_right;

void speed_init() {
  speed_pi_left.min = 0;
  speed_pi_left.max = 200;
  speed_pi_left.kp  = 0.3;
  speed_pi_left.ki  = 0.04;

  speed_pi_right.min = 0;
  speed_pi_right.max = 200;
  speed_pi_right.kp  = 0.3;
  speed_pi_right.ki  = 0.04;
}

#define LOW_PASS_ALPHA 0.1

void speed_read() {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    speed_measured_left = (LOW_PASS_ALPHA * ((float)encoders_left_delta) * 50.0)  //
                        + ((1.0 - LOW_PASS_ALPHA) * speed_measured_left);
    speed_measured_right = (LOW_PASS_ALPHA * ((float)encoders_right_delta) * 50.0)  //
                         + ((1.0 - LOW_PASS_ALPHA) * speed_measured_right);
    encoders_update();
  }
}

void speed_update() {
  uint8_t power_left;
  if (fabsf(speed_setpoint_left) < MIN_SPEED) {
    pi_reset(&speed_pi_left);
    power_left = 0;
  } else {
    power_left = (uint8_t)pi_update(&speed_pi_left, fabsf(speed_setpoint_left), fabsf(speed_measured_left));
    if (power_left != 0) {
      power_left += 26;
    }
  }

  uint8_t power_right;
  if (fabsf(speed_setpoint_right) < MIN_SPEED) {
    pi_reset(&speed_pi_right);
    power_right = 0;
  } else {
    power_right = (uint8_t)pi_update(&speed_pi_right, fabsf(speed_setpoint_right), fabsf(speed_measured_right));
    if (power_right != 0) {
      power_right += 26;
    }
  }

  bool forward_left  = signbitf(speed_setpoint_left) == 0;
  bool forward_right = signbitf(speed_setpoint_right) == 0;
  motor_set(forward_left ? power_left : -power_left,  //
            forward_right ? power_right : -power_right);
}

void speed_set(float left, float right) {
  speed_setpoint_left  = left;
  speed_setpoint_right = right;
}

void speed_set_pi_vars(float kp, float ki) {
  speed_pi_left.kp = kp;
  speed_pi_left.ki = ki;

  speed_pi_right.kp = kp;
  speed_pi_right.ki = ki;
}
