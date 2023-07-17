#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "sensor_cal.hh"

namespace sensor_cal {

// init initializes the sensor calibration module.
void init();

// start starts a sensor calibration plan.
void start();

// tick updates the platform to perform the calibration.
bool tick();

// read reads the calibrated sensor values.
void read(uint16_t *left, uint16_t *right, uint16_t *forward);

}
