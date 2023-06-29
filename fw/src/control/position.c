#include "position.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/atomic.h>

#include "control/pid.h"
#include "control/speed.h"
#include "platform/encoders.h"
#include "platform/motor.h"
#include "platform/pin.h"
#include "platform/rtc.h"

// Motor positions in RPM.
float position_measured_left;
float position_measured_right;

// Motor position setpoints.
float position_setpoint_left;
float position_setpoint_right;

static bool  position_enabled;
static pid_t position_pid_left;
static pid_t position_pid_right;

void position_init() {
  position_pid_left.min = -200;
  position_pid_left.max = 200;
  position_pid_left.kp  = 8;
  position_pid_left.ki  = 0.05;
  position_pid_left.kd  = 0.01;

  position_pid_right.min = -200;
  position_pid_right.max = 200;
  position_pid_right.kp  = 8;
  position_pid_right.ki  = 0.05;
  position_pid_right.kd  = 0.01;

  position_setpoint_left  = 500;
  position_setpoint_right = 500;
}

void position_update() {
  uint16_t l, r;
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    l = encoder_left;
    r = encoder_right;
  }
  position_measured_left  = ((float)l) * ((32.0 * M_PI) / 240.0);
  position_measured_right = ((float)r) * ((32.0 * M_PI) / 240.0);

  if (position_enabled) {
    speed_set_left(pid_update(&position_pid_left, position_setpoint_left, position_measured_left));
    speed_set_right(pid_update(&position_pid_right, position_setpoint_right, position_measured_right));
  }

  speed_update();
}

void position_enable() {
  position_enabled = true;
  speed_enable();
}

void position_disable() {
  position_enabled = false;
  speed_disable();
}

void position_set_left(float setpoint) {
  position_setpoint_left = setpoint;
}

void position_set_right(float setpoint) {
  position_setpoint_right = setpoint;
}
