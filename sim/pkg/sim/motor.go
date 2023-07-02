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

type EncoderTickEvent struct {
	Left, Clockwise bool
}

type Motor struct {
	DesiredPeriod                uint32
	ActualPeriod                 uint32
	Clockwise                    bool
	left                         bool
	ix                           int // mod 4 location in pulse train
	avr                          *C.avr_t
	pwmIRQ, clkIRQ, bIRQ, dirIRQ *C.avr_irq_t
	encoderChan                  chan<- EncoderTickEvent
}

func NewMotor(avr *C.avr_t, left bool, encoderChan chan<- EncoderTickEvent) *Motor {
	m := &Motor{
		DesiredPeriod: 0,
		ActualPeriod:  0,
		Clockwise:     !left,
		left:          left,
		ix:            0,
		avr:           avr,
		encoderChan:   encoderChan,
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

const LEFT_MOTOR_M float32 = 2.760
const LEFT_MOTOR_B float32 = -67.2
const RIGHT_MOTOR_M float32 = 2.695
const RIGHT_MOTOR_B float32 = -57.7

func (m *Motor) OnIRQ(irq *C.avr_irq_t, value uint32, param unsafe.Pointer) {
	if irq == m.pwmIRQ {
		if value < 35 {
			// Not enough power to start the motor turning.
			m.DesiredPeriod = 0
		} else {
			var rpm float32
			if m.left {
				rpm = (float32(value) * LEFT_MOTOR_M) + LEFT_MOTOR_B
			} else {
				rpm = (float32(value) * RIGHT_MOTOR_M) + RIGHT_MOTOR_B
			}
			period := (16000000.0 * 60.0 / 240.0) / rpm
			m.DesiredPeriod = uint32(period)
		}
	} else if irq == m.dirIRQ {
		m.Clockwise = value != 0
	}
}

func (m *Motor) OnCycle(avr *C.avr_t, when C.avr_cycle_count_t, param unsafe.Pointer) C.avr_cycle_count_t {
	if m.ActualPeriod > 0 {
		if m.Clockwise {
			m.ix = (m.ix + 1) % 4
		} else {
			m.ix = (m.ix + 3) % 4
		}
		m.encoderChan <- EncoderTickEvent{Left: m.left, Clockwise: m.Clockwise}
	}

	C.avr_raise_irq(m.bIRQ, C.uint((m.ix>>1)&1))
	C.avr_raise_irq(m.clkIRQ, C.uint(m.ix&1))

	m.ActualPeriod = m.DesiredPeriod
	if m.ActualPeriod == 0 {
		return when + 10000
	}

	return when + C.avr_cycle_count_t(m.ActualPeriod)
}
