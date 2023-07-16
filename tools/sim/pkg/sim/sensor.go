package sim

/*
#cgo pkg-config: simavr simavrparts

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

type Sensor struct {
	Voltage int32
	Name    string
	Pos     Pos // relative to the center of the mouse's axle
	Angle   float64
	avr     *C.avr_t
	irq     *C.avr_irq_t
}

func NewSensor(avr *C.avr_t, name string, adc C.int, pos Pos, angle float64) *Sensor {
	return &Sensor{
		Voltage: 0,
		Name:    name,
		Pos:     pos,
		Angle:   angle,
		avr:     avr,
		irq:     C.avr_io_getirq(avr, C.AVR_IOCTL_ADC_GETIRQ, adc),
	}
}

func (s *Sensor) Init() {
	C.avr_vcd_add_signal(s.avr.vcd, s.irq, 16, C.CString(s.Name))
	C.avr_irq_register_notify(C.avr_io_getirq(s.avr, C.AVR_IOCTL_ADC_GETIRQ, C.ADC_IRQ_OUT_TRIGGER), on_irq_cgo, pointer.Save(s))
}

func (s *Sensor) OnIRQ(irq *C.avr_irq_t, value uint32, param unsafe.Pointer) {
	C.avr_raise_irq(s.irq, C.uint(s.Voltage))
}
