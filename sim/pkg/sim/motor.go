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
*/
import "C"

import (
	"fmt"
	"unsafe"

	"github.com/mattn/go-pointer"
)

type Motor struct {
	avr *C.avr_t
	irq *C.avr_irq_t
}

func NewMotor(avr *C.avr_t, pwm C.int) *Motor {
	return &Motor{
		avr: avr,
		irq: C.avr_io_getirq(avr, 0x746D7231 /* tmr1 */, pwm),
	}
}

func (m *Motor) Init() {
	C.avr_irq_register_notify(m.irq, on_irq_cgo, pointer.Save(m))
}

func (m *Motor) OnIRQ(irq *C.avr_irq_t, value uint32, param unsafe.Pointer) {
	fmt.Printf("PWM changed: %d\n", value)
}
