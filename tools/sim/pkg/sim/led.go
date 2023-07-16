package sim

/*
#cgo pkg-config: simavr simavrparts

#include <avr_ioport.h>
#include <sim_avr.h>
*/
import "C"

import (
	"unsafe"

	"github.com/mattn/go-pointer"
)

type LEDs struct {
	Right, Left, IR, Onboard bool
	avr                      *C.avr_t
	events                   chan<- struct{}
}

func NewLEDs(avr *C.avr_t, events chan<- struct{}) *LEDs {
	return &LEDs{
		avr:    avr,
		events: events,
	}
}

func (l *LEDs) Init() {
	C.avr_register_io_write(l.avr, C.ushort(0x25), on_io_write_cgo, pointer.Save(l))
	C.avr_register_io_write(l.avr, C.ushort(0x2B), on_io_write_cgo, pointer.Save(l))
}

func (l *LEDs) OnIOWrite(avr *C.avr_t, addr C.avr_io_addr_t, v uint8, param unsafe.Pointer) {
	switch addr {
	case 0x25:
		l.Left = (v>>3)&1 == 1
		l.IR = (v>>4)&1 == 1
		l.Onboard = (v>>5)&1 == 1
	case 0x2B:
		l.Right = (v>>6)&1 == 1
	}
	l.events <- struct{}{}
}
