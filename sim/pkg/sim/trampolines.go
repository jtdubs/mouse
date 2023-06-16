package sim

/*
#include <avr_adc.h>
#include <avr_ioport.h>
#include <sim_avr.h>

void on_irq(struct avr_irq_t *irq, uint32_t value, void *param);
void on_io_write(struct avr_t *avr, avr_io_addr_t addr, uint8_t v, void *param);

__attribute__((weak))
void on_irq_cgo(struct avr_irq_t *irq, uint32_t value, void *param) {
	on_irq(irq, value, param);
}

__attribute__((weak))
void on_io_write_cgo(struct avr_t *avr, avr_io_addr_t addr, uint8_t v, void *param) {
	on_io_write(avr, addr, v, param);
}
*/
import "C"

import (
	"unsafe"

	"github.com/mattn/go-pointer"
)

type IRQHandler interface {
	OnIRQ(irq *C.avr_irq_t, value uint32, param unsafe.Pointer)
}

type IOWriteHandler interface {
	OnIOWrite(irq *C.avr_t, addr C.avr_io_addr_t, value uint8, param unsafe.Pointer)
}

var (
	on_io_write_cgo = C.avr_io_write_t(C.on_io_write_cgo)
	on_irq_cgo      = C.avr_irq_notify_t(C.on_irq_cgo)
)

//export on_irq
func on_irq(irq *C.avr_irq_t, value uint32, param unsafe.Pointer) {
	pointer.Restore(param).(IRQHandler).OnIRQ(irq, value, param)
}

//export on_io_write
func on_io_write(avr *C.avr_t, addr C.avr_io_addr_t, v uint8, param unsafe.Pointer) {
	pointer.Restore(param).(IOWriteHandler).OnIOWrite(avr, addr, v, param)
}
