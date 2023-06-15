#pragma once

#include <stdbool.h>

#include "pin.h"

// pwm_init initializes pwm.
void pwm_init();

void set_pwm_duty_cycle_a(uint8_t duty_cycle);
void set_pwm_duty_cycle_b(uint8_t duty_cycle);
