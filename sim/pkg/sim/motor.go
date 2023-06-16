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
	speed                uint32
	avr                  *C.avr_t
	pwmIRQ, bIRQ, clkIRQ *C.avr_irq_t
}

func NewMotor(avr *C.avr_t, name string, pwm C.int, bPin int, clkPin int) *Motor {
	return &Motor{
		name:   name,
		speed:  0,
		avr:    avr,
		pwmIRQ: C.avr_io_getirq(avr, 0x746D7231 /* tmr1 */, pwm),
		bIRQ:   C.avr_io_getirq(avr, 0x696F6744 /* iogD */, C.int(bPin)),
		clkIRQ: C.avr_io_getirq(avr, 0x696F6744 /* iogD */, C.int(clkPin)),
	}
}

func (m *Motor) Init() {
	C.avr_irq_register_notify(m.pwmIRQ, on_irq_cgo, pointer.Save(m))
}

func (m *Motor) OnIRQ(irq *C.avr_irq_t, value uint32, param unsafe.Pointer) {
	fmt.Printf("%v PWM changed: %d\n", m.name, value)
	m.speed = value
	C.avr_cycle_timer_register(m.avr, m.avr.cycle+400, on_cycle_cgo, pointer.Save(m))
}

func (m *Motor) OnCycle(avr *C.avr_t, when C.avr_cycle_count_t, param unsafe.Pointer) {
	fmt.Printf("%v Cycle!\n", m.name)
	// C.avr_raise_irq(m.clkIRQ, 5000)
	// C.avr_raise_irq(m.bIRQ, 5000)
}
