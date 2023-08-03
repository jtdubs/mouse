#pragma once

#include <avr/io.h>

namespace mouse::platform::pin {

// Pin represents an output pin on the atmega328p.
struct Pin {
  volatile uint8_t* out;
  volatile uint8_t* set;
  volatile uint8_t* clr;
  volatile uint8_t* tgl;
  uint8_t           mask;
};

const Pin kRightDir = {
    .out = &PORTE_OUT, .set = &PORTE_OUTSET, .clr = &PORTE_OUTCLR, .tgl = &PORTE_OUTTGL, .mask = _BV(3)};  // D8 (PE3)
const Pin kLeftPWM = {
    .out = &PORTB_OUT, .set = &PORTB_OUTSET, .clr = &PORTB_OUTCLR, .tgl = &PORTB_OUTTGL, .mask = _BV(0)};  // D9 (PB0)
const Pin kRightPWM = {
    .out = &PORTB_OUT, .set = &PORTB_OUTSET, .clr = &PORTB_OUTCLR, .tgl = &PORTB_OUTTGL, .mask = _BV(1)};  // D10 (PB1)
const Pin kLEDLeft = {
    .out = &PORTE_OUT, .set = &PORTE_OUTSET, .clr = &PORTE_OUTCLR, .tgl = &PORTE_OUTTGL, .mask = _BV(0)};  // D11 (PE0)
const Pin kIRLEDs = {
    .out = &PORTE_OUT, .set = &PORTE_OUTSET, .clr = &PORTE_OUTCLR, .tgl = &PORTE_OUTTGL, .mask = _BV(1)};  // D12 (PE1)
const Pin kLEDOnboard = {
    .out = &PORTE_OUT, .set = &PORTE_OUTSET, .clr = &PORTE_OUTCLR, .tgl = &PORTE_OUTTGL, .mask = _BV(2)};  // D13 (PE2)
const Pin kProbeTick = {
    .out = &PORTD_OUT, .set = &PORTD_OUTSET, .clr = &PORTD_OUTCLR, .tgl = &PORTD_OUTTGL, .mask = _BV(0)};  // A3 (PD0)
const Pin kProbePlan = {
    .out = &PORTA_OUT, .set = &PORTA_OUTSET, .clr = &PORTA_OUTCLR, .tgl = &PORTA_OUTTGL, .mask = _BV(2)};  // A4 (PA2)
const Pin kProbeUnused = {
    .out = &PORTA_OUT, .set = &PORTA_OUTSET, .clr = &PORTA_OUTCLR, .tgl = &PORTA_OUTTGL, .mask = _BV(3)};  // A5 (PA3)
const Pin kLEDRight = {
    .out = &PORTF_OUT, .set = &PORTF_OUTSET, .clr = &PORTF_OUTCLR, .tgl = &PORTF_OUTTGL, .mask = _BV(4)};  // D6 (PF4)
const Pin kLeftDir = {
    .out = &PORTA_OUT, .set = &PORTA_OUTSET, .clr = &PORTA_OUTCLR, .tgl = &PORTA_OUTTGL, .mask = _BV(1)};  // D7 (PA1)

void Init();

// Set sets the value of the pin.
inline void Set(Pin pin, bool on) {
  if (on) {
    *pin.set = pin.mask;
  } else {
    *pin.clr = pin.mask;
  }
}

// set sets the pin to high.
inline void Set(Pin pin) {
  *pin.set = pin.mask;
}

// Clear sets the pin to low.
inline void Clear(Pin pin) {
  *pin.clr = pin.mask;
}

// toggle toggles the pin.
inline void Toggle(Pin pin) {
  *pin.tgl = pin.mask;
}

// IsSet checks if the pin is set.
inline bool IsSet(Pin pin) {
  return *pin.out & pin.mask;
}

}  // namespace mouse::platform::pin
