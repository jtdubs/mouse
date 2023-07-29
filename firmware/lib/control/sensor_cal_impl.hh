#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "sensor_cal.hh"

namespace mouse::control::sensor_cal {

// Init initializes the sensor calibration module.
void Init();

// start starts a sensor calibration plan.
void Start();

// Tick updates the platform to perform the calibration.
bool Tick();

// read reads the calibrated sensor values.
void Read(uint16_t &left, uint16_t &right, uint16_t &forward);

}  // namespace mouse::control::sensor_cal
