#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/math.hh"
#include "motor_impl.hh"
#include "pin_impl.hh"

namespace mouse::platform::motor {

namespace {
int16_t power_left_;
int16_t power_right_;
}  // namespace

// Init initializes the motors.
void Init() {
  // TODO
  power_left_  = 0;
  power_right_ = 0;
}

// set sets the direction of the motors.
void Set(int16_t left, int16_t right) {
  assert(assert::Module::Motor, 0, left > -512 && left < 512);
  assert(assert::Module::Motor, 1, right > -512 && right < 512);
  // TODO
}

// read reads the power levels of the motors.
// Range: [-511, 511]
void Read(int16_t& left, int16_t& right) {
  // TODO
  left  = 0;
  right = 0;
}

}  // namespace mouse::platform::motor
