#pragma once

#include <stdbool.h>
#include <stdint.h>

// Encoder counts.
extern int32_t encoders_left;
extern int32_t encoders_right;

// Changes to encoder counts since the last update.
extern int8_t encoders_left_delta;
extern int8_t encoders_right_delta;

// encoders_init initializes the encoders.
void encoders_init();

// encoders_update applies changes since the last update.
void encoders_update();
