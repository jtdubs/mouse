#pragma once

#include <stdint.h>

// explore() is the entry point for the explore mode.
void explore();

// explore_report() is the report handler for the explore mode.
uint8_t explore_report(uint8_t *buffer, uint8_t len);
