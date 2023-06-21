package sim

/*
#cgo CFLAGS: -I/usr/local/include/simavr -I/usr/local/include/simavr/parts
#cgo LDFLAGS: -L/usr/local/lib -lelf -lsimavrparts -lsimavr

#include <avr_adc.h>
#include <avr_ioport.h>
#include <sim_avr.h>
#include <sim_vcd_file.h>
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
	C.avr_vcd_add_signal(b.avr.vcd, b.irq, 16, C.CString("BATTERY_VOLTAGE"))
	C.avr_irq_register_notify(C.avr_io_getirq(b.avr, C.AVR_IOCTL_ADC_GETIRQ, C.ADC_IRQ_OUT_TRIGGER), on_irq_cgo, pointer.Save(b))
}

func (b *Battery) OnIRQ(irq *C.avr_irq_t, value uint32, param unsafe.Pointer) {
	C.avr_raise_irq(b.irq, C.uint(b.Voltage/2))
}
