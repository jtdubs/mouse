package sim

/*
#cgo pkg-config: simavr simavrparts

#include <avr_adc.h>
#include <avr_ioport.h>
#include <sim_avr.h>

void on_irq(struct avr_irq_t *irq, uint32_t value, void *param);
void on_io_write(struct avr_t *avr, avr_io_addr_t addr, uint8_t v, void *param);
avr_cycle_count_t on_cycle(struct avr_t *avr, avr_cycle_count_t when, void *param);

__attribute__((weak))
void on_irq_cgo(struct avr_irq_t *irq, uint32_t value, void *param) {
	on_irq(irq, value, param);
}

__attribute__((weak))
void on_io_write_cgo(struct avr_t *avr, avr_io_addr_t addr, uint8_t v, void *param) {
	on_io_write(avr, addr, v, param);
}

__attribute__((weak))
avr_cycle_count_t on_cycle_cgo(struct avr_t *avr, avr_cycle_count_t when, void *param) {
	return on_cycle(avr, when, param);
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
	OnIOWrite(avr *C.avr_t, addr C.avr_io_addr_t, value uint8, param unsafe.Pointer)
}

type CycleHandler interface {
	OnCycle(avr *C.avr_t, when C.avr_cycle_count_t, param unsafe.Pointer) C.avr_cycle_count_t
}

var (
	on_io_write_cgo = C.avr_io_write_t(C.on_io_write_cgo)
	on_irq_cgo      = C.avr_irq_notify_t(C.on_irq_cgo)
	on_cycle_cgo    = C.avr_cycle_timer_t(C.on_cycle_cgo)
)

//export on_irq
func on_irq(irq *C.avr_irq_t, value uint32, param unsafe.Pointer) {
	pointer.Restore(param).(IRQHandler).OnIRQ(irq, value, param)
}

//export on_io_write
func on_io_write(avr *C.avr_t, addr C.avr_io_addr_t, v uint8, param unsafe.Pointer) {
	pointer.Restore(param).(IOWriteHandler).OnIOWrite(avr, addr, v, param)
}

//export on_cycle
func on_cycle(avr *C.avr_t, when C.avr_cycle_count_t, param unsafe.Pointer) C.avr_cycle_count_t {
	return pointer.Restore(param).(CycleHandler).OnCycle(avr, when, param)
}
