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

type FunctionSelector struct {
	Function      int32
	ButtonPressed bool
	avr           *C.avr_t
	irq           *C.avr_irq_t
}

func NewFunctionSelect(avr *C.avr_t, adc C.int) *FunctionSelector {
	return &FunctionSelector{
		Function:      0,
		ButtonPressed: false,
		avr:           avr,
		irq:           C.avr_io_getirq(avr, C.AVR_IOCTL_ADC_GETIRQ, adc),
	}
}

func (f *FunctionSelector) Init() {
	C.avr_vcd_add_signal(f.avr.vcd, f.irq, 16, C.CString("FUNCTION_SELECT"))
	C.avr_irq_register_notify(C.avr_io_getirq(f.avr, C.AVR_IOCTL_ADC_GETIRQ, C.ADC_IRQ_OUT_TRIGGER), on_irq_cgo, pointer.Save(f))
}

var functionVoltages = [16]int{
	3235, 3176, 3098, 3019, 2882, 2784, 2666, 2549, 2274, 2117, 1901, 1686, 1333, 1039, 627, 215,
}

func (f *FunctionSelector) OnIRQ(irq *C.avr_irq_t, value uint32, param unsafe.Pointer) {
	if f.ButtonPressed {
		C.avr_raise_irq(f.irq, C.uint(5000))
	} else {
		C.avr_raise_irq(f.irq, C.uint(functionVoltages[f.Function]))
	}
}
