#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "firmware/lib/utils/assert.hh"
#include "firmware/lib/utils/math.hh"
#include "motor_impl.hh"
#include "pin_impl.hh"

namespace motor {

namespace {
int16_t power_left;
int16_t power_right;
}  // namespace

// init initializes the motors.
void init() {
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

  set(0, 0);
}

// set sets the direction of the motors.
void set(int16_t left, int16_t right) {
  assert(assert::MOTOR + 0, left > -512 && left < 512);
  assert(assert::MOTOR + 1, right > -512 && right < 512);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    power_left  = left;
    power_right = right;

    // set the direction of the motors
    pin::set2(pin::LEFT_DIR, left < 0);
    pin::set2(pin::RIGHT_DIR, right >= 0);

    // set the PWM duty cycle for each motor
    OCR1A = math::abs16(left);
    OCR1B = math::abs16(right);
  }
}

// read reads the power levels of the motors.
// Range: [-511, 511]
void read(int16_t& left, int16_t& right) {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    left  = power_left;
    right = power_right;
  }
}

}  // namespace motor
