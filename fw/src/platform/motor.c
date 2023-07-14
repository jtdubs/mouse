#include "motor.h"

#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "platform/pin.h"
#include "utils/assert.h"
#include "utils/math.h"

static int16_t motor_power_left;
static int16_t motor_power_right;

// motor_init initializes the motors.
void motor_init() {
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

  motor_set(0, 0);
}

// motor_set sets the direction of the motors.
void motor_set(int16_t left, int16_t right) {
  assert(ASSERT_MOTOR + 0, left > -512 && left < 512);
  assert(ASSERT_MOTOR + 1, right > -512 && right < 512);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    motor_power_left  = left;
    motor_power_right = right;

    // set the direction of the motors
    pin_set2(LEFT_DIR, left < 0);
    pin_set2(RIGHT_DIR, right >= 0);

    // set the PWM duty cycle for each motor
    OCR1A = abs16(left);
    OCR1B = abs16(right);
  }
}

// motor_read reads the power levels of the motors.
// Range: [-511, 511]
void motor_read(int16_t* left, int16_t* right) {
  assert(ASSERT_MOTOR + 2, left != NULL);
  assert(ASSERT_MOTOR + 3, right != NULL);

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    *left  = motor_power_left;
    *right = motor_power_right;
  }
}
