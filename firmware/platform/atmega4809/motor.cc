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
  // TODO(justindubs): 4809 impl
  TCCR1A = _BV(COM1A1)   // Clear OC1A when up-counting, set when down-counting
         | _BV(COM1B1);  // Clear OCBA when up-counting, set when down-counting
  TCCR1B = _BV(WGM13)    // Phase and frequency correct PWM
         | _BV(CS10);    // Use internal clock (16Mhz)
  ICR1   = 510;          // ~16kHz PWM frequency (15MHz / 510 / 2)
  OCR1A  = 0;            // 0% duty cycle
  OCR1B  = 0;            // 0% duty cycle
  TIMSK1 = 0;            // Disable interrupts.
  TIFR0  = 0;            // Clear interrupt flags.
  TCNT1  = 0;            // Reset counter.

  Set(0, 0);
}

// set sets the direction of the motors.
void Set(int16_t left, int16_t right) {
  assert(assert::Module::Motor, 0, left > -512 && left < 512);
  assert(assert::Module::Motor, 1, right > -512 && right < 512);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    power_left_  = left;
    power_right_ = right;

    // set the direction of the motors
    pin::Set(pin::kLeftDir, left < 0);
    pin::Set(pin::kRightDir, right >= 0);

    // set the PWM duty cycle for each motor
    // TODO(justindubs): 4809 impl
    OCR1A = math::abs16(left);
    OCR1B = math::abs16(right);
  }
}

// read reads the power levels of the motors.
// Range: [-511, 511]
void Read(int16_t& left, int16_t& right) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left  = power_left_;
    right = power_right_;
  }
}

}  // namespace mouse::platform::motor
