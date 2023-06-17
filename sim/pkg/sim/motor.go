package sim

/*
#cgo CFLAGS: -I/usr/include/simavr
#cgo CFLAGS: -I/usr/include/simavr/parts
#cgo CFLAGS: -I.
#cgo LDFLAGS: -L. -lsimavr -lsimavrparts

#include <avr_adc.h>
#include <avr_ioport.h>
#include <avr_timer.h>
#include <sim_avr.h>
#include <sim_cycle_timers.h>
*/
import "C"

import (
	"fmt"
	"unsafe"

	"github.com/mattn/go-pointer"
)

type Motor struct {
	name                 string
	DesiredFrequency     uint32
	ActualFrequency      uint32
	ix                   int // mod 4 location in pulse train
	avr                  *C.avr_t
	pwmIRQ, clkIRQ, bIRQ *C.avr_irq_t
}

func NewMotor(avr *C.avr_t, name string, pwm C.int, clkPin int, bPin int) *Motor {
	return &Motor{
		name:             name,
		DesiredFrequency: 0,
		ActualFrequency:  0,
		ix:               0,
		avr:              avr,
		pwmIRQ:           C.avr_io_getirq(avr, 0x746D7231 /* tmr1 */, pwm),
		clkIRQ:           C.avr_io_getirq(avr, 0x696F6744 /* iogD */, C.int(clkPin)),
		bIRQ:             C.avr_io_getirq(avr, 0x696F6744 /* iogD */, C.int(bPin)),
	}
}

func (m *Motor) Init() {
	C.avr_irq_register_notify(m.pwmIRQ, on_irq_cgo, pointer.Save(m))
	C.avr_cycle_timer_register(m.avr, m.avr.cycle+10000, on_cycle_cgo, pointer.Save(m))
}

func (m *Motor) OnIRQ(irq *C.avr_irq_t, value uint32, param unsafe.Pointer) {
	fmt.Printf("%v PWM changed: %d\n", m.name, value)
	// As a crude approximation, we'll map the PWM range [0,512) to motor speeds of [0,32) Hz.
	// Which means the encoder pulses run at [0,128) Hz, or PWM >> 2.
	m.DesiredFrequency = value >> 2
}

func (m *Motor) OnCycle(avr *C.avr_t, when C.avr_cycle_count_t, param unsafe.Pointer) C.avr_cycle_count_t {
	if m.ActualFrequency > 0 {
		m.ix = (m.ix + 1) % 4
	}

	C.avr_raise_irq(m.clkIRQ, C.uint(m.ix&1))
	C.avr_raise_irq(m.bIRQ, C.uint((m.ix>>1)&1))

	m.ActualFrequency = m.DesiredFrequency
	if m.ActualFrequency == 0 {
		return when + 10000
	}

	return when + C.avr_cycle_count_t(16000000/m.ActualFrequency)
}
