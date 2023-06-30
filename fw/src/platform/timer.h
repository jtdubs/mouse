#pragma once

typedef void (*timer_callback_t)();

void timer_set_callback(timer_callback_t callback);

// timer_init initializes timer.
void timer_init();
