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

static bool speed_enabled;
pid_t       speed_pid_left;
pid_t       speed_pid_right;

void speed_init() {
  speed_pid_left.min = 0;
  speed_pid_left.max = 200;
  speed_pid_left.kp  = 0.3;
  speed_pid_left.ki  = 0.04;
  speed_pid_left.kd  = 0;

  speed_pid_right.min = 0;
  speed_pid_right.max = 200;
  speed_pid_right.kp  = 0.3;
  speed_pid_right.ki  = 0.04;
  speed_pid_right.kd  = 0;
}

#define LOW_PASS_ALPHA 0.1

void speed_update() {
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    speed_measured_left = (LOW_PASS_ALPHA * ((float)encoders_left_delta) * 50.0)  //
                        + ((1.0 - LOW_PASS_ALPHA) * speed_measured_left);
    speed_measured_right = (LOW_PASS_ALPHA * ((float)encoders_right_delta) * 50.0)  //
                         + ((1.0 - LOW_PASS_ALPHA) * speed_measured_right);
    encoders_update();
  }

  if (speed_enabled) {
    uint8_t power_left;
    if (fabsf(speed_setpoint_left) < MIN_SPEED) {
      pid_reset(&speed_pid_left);
      power_left = 0;
    } else {
      power_left = (uint8_t)pid_update(&speed_pid_left, fabsf(speed_setpoint_left), fabsf(speed_measured_left));
      if (power_left != 0) {
        power_left += 26;
      }
    }

    uint8_t power_right;
    if (fabsf(speed_setpoint_right) < MIN_SPEED) {
      pid_reset(&speed_pid_right);
      power_right = 0;
    } else {
      power_right = (uint8_t)pid_update(&speed_pid_right, fabsf(speed_setpoint_right), fabsf(speed_measured_right));
      if (power_right != 0) {
        power_right += 26;
      }
    }

    bool forward_left  = signbitf(speed_setpoint_left) == 0;
    bool forward_right = signbitf(speed_setpoint_right) == 0;

    motor_set_forward_left(forward_left);
    motor_set_power_left(power_left);
    motor_set_forward_right(forward_right);
    motor_set_power_right(power_right);
  }
}

void speed_enable() {
  speed_enabled = true;
}

void speed_disable() {
  speed_enabled = false;
}

void speed_set_left(float setpoint) {
  speed_setpoint_left = setpoint;
}

void speed_set_right(float setpoint) {
  speed_setpoint_right = setpoint;
}

void speed_set_pid_vars(float kp, float ki, float kd) {
  speed_pid_left.kp = kp;
  speed_pid_left.ki = ki;
  speed_pid_left.kd = kd;

  speed_pid_right.kp = kp;
  speed_pid_right.ki = ki;
  speed_pid_right.kd = kd;
}
