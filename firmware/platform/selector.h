//
// System: platform
// Module: selector
//
// Purpose:
// - Interprets the state of the selector switch and button.
// - Provides a function to read the selected state (if it has changed)
//
#pragma once

#include <stdint.h>

// selector_update updates the selector state, returning the selected value
// if the button has been pressed, and 0xFF is no button press has occurred.
uint8_t selector_update();
