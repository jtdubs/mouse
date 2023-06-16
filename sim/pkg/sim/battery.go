package sim

/*
#cgo CFLAGS: -I/usr/include/simavr
#cgo CFLAGS: -I/usr/include/simavr/parts
#cgo CFLAGS: -I.
#cgo LDFLAGS: -L. -lsimavr -lsimavrparts

#include <avr_adc.h>
#include <avr_ioport.h>
#include <sim_avr.h>
*/
import "C"

import (
	"unsafe"

	"github.com/mattn/go-pointer"
)

type Battery struct {
	Voltage int32
	avr     *C.avr_t
	irq     *C.avr_irq_t
}

func NewBattery(avr *C.avr_t, adc C.int) *Battery {
	return &Battery{
		Voltage: 9000,
		avr:     avr,
		irq:     C.avr_io_getirq(avr, C.AVR_IOCTL_ADC_GETIRQ, adc),
	}
}

func (b *Battery) Init() {
	C.avr_irq_register_notify(C.avr_io_getirq(b.avr, C.AVR_IOCTL_ADC_GETIRQ, C.ADC_IRQ_OUT_TRIGGER), on_irq_cgo, pointer.Save(b))
}

func (b *Battery) OnIRQ(irq *C.avr_irq_t, value uint32, param unsafe.Pointer) {
	C.avr_raise_irq(b.irq, C.uint(b.Voltage/2))
}
