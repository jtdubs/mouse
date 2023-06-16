package sim

/*
#cgo CFLAGS: -I/usr/include/simavr
#cgo CFLAGS: -I/usr/include/simavr/parts
#cgo CFLAGS: -I.
#cgo LDFLAGS: -L. -lsimavr -lsimavrparts

#include <avr_ioport.h>
#include <sim_avr.h>
*/
import "C"

import (
	"unsafe"

	"github.com/mattn/go-pointer"
)

type LED struct {
	State bool
	avr   *C.avr_t
	addr  int
	bit   int
}

func NewLED(avr *C.avr_t, addr, bit int) *LED {
	return &LED{
		State: false,
		avr:   avr,
		addr:  addr,
		bit:   bit,
	}
}

func (l *LED) Init() {
	C.avr_register_io_write(l.avr, C.ushort(l.addr), on_io_write_cgo, pointer.Save(l))
}

func (l *LED) OnIOWrite(avr *C.avr_t, addr C.avr_io_addr_t, v uint8, param unsafe.Pointer) {
	l.State = (v>>l.bit)&1 == 1
}
