package sim

/*
#cgo pkg-config: simavr simavrparts

#include <avr_adc.h>
#include <avr_ioport.h>
#include <avr_timer.h>
#include <sim_avr.h>
#include <sim_cycle_timers.h>
*/
import "C"

import (
	"unsafe"

	"github.com/mattn/go-pointer"
)

type Motor struct {
	DesiredFrequency             uint32
	ActualFrequency              uint32
	Forward                      bool
	left                         bool
	ix                           int // mod 4 location in pulse train
	avr                          *C.avr_t
	pwmIRQ, clkIRQ, bIRQ, dirIRQ *C.avr_irq_t
}

func NewMotor(avr *C.avr_t, left bool) *Motor {
	m := &Motor{
		DesiredFrequency: 0,
		ActualFrequency:  0,
		Forward:          true,
		left:             left,
		ix:               0,
		avr:              avr,
	}
	if left {
		m.pwmIRQ = C.avr_io_getirq(avr, 0x746D7231 /* tmr1 */, 0)
		m.clkIRQ = C.avr_io_getirq(avr, 0x696F6744 /* iogD */, C.int(2))
		m.bIRQ = C.avr_io_getirq(avr, 0x696F6744 /* iogD */, C.int(4))
		m.dirIRQ = C.avr_io_getirq(avr, 0x696F6744 /* iogD */, C.int(7))
	} else {
		m.pwmIRQ = C.avr_io_getirq(avr, 0x746D7231 /* tmr1 */, 1)
		m.clkIRQ = C.avr_io_getirq(avr, 0x696F6744 /* iogD */, C.int(3))
		m.bIRQ = C.avr_io_getirq(avr, 0x696F6744 /* iogD */, C.int(5))
		m.dirIRQ = C.avr_io_getirq(avr, 0x696F6742 /* iogB */, C.int(0))
	}
	return m
}

func (m *Motor) Init() {
	C.avr_irq_register_notify(m.pwmIRQ, on_irq_cgo, pointer.Save(m))
	C.avr_irq_register_notify(m.dirIRQ, on_irq_cgo, pointer.Save(m))
	C.avr_io_getirq(m.avr, 0x696F6744 /* iogD */, 0) // set pin to output
	C.avr_cycle_timer_register(m.avr, m.avr.cycle+10000, on_cycle_cgo, pointer.Save(m))
}

func (m *Motor) OnIRQ(irq *C.avr_irq_t, value uint32, param unsafe.Pointer) {
	if irq == m.pwmIRQ {
		// As a crude approximation, we'll map the PWM range [0,512) to motor speeds of [0,32) Hz.
		// Which means the encoder pulses run at [0,128) Hz, or PWM >> 2.
		m.DesiredFrequency = value >> 2
	} else if irq == m.dirIRQ {
		m.Forward = value != 0
	}
}

func (m *Motor) OnCycle(avr *C.avr_t, when C.avr_cycle_count_t, param unsafe.Pointer) C.avr_cycle_count_t {
	if m.ActualFrequency > 0 {
		if m.Forward != m.left {
			m.ix = (m.ix + 1) % 4
		} else {
			m.ix = (m.ix + 3) % 4
		}
	}

	C.avr_raise_irq(m.bIRQ, C.uint((m.ix>>1)&1))
	C.avr_raise_irq(m.clkIRQ, C.uint(m.ix&1))

	m.ActualFrequency = m.DesiredFrequency
	if m.ActualFrequency == 0 {
		return when + 10000
	}

	return when + C.avr_cycle_count_t(16000000/m.ActualFrequency)
}
