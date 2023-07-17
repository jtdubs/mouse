//
// System: control
// Module: sensor_cal
//
// Purpose:
// - Handles sensor calibration plans.
//
// Design:
// - Enables IR LEDs for 256 ticks, and averages readings over that period.
// - This average is used as the baseline for wall detection.
//
#pragma once
