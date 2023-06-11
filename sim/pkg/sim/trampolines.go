package sim

/*
#include <avr_adc.h>
#include <avr_ioport.h>
#include <sim_avr.h>

void on_adc_irq(struct avr_irq_t *irq, uint32_t value, void *param);
void on_led_write(struct avr_t *avr, avr_io_addr_t addr, uint8_t v, void *param);

__attribute__((weak))
void on_adc_irq_cgo(struct avr_irq_t *irq, uint32_t value, void *param) {
	on_adc_irq(irq, value, param);
}

__attribute__((weak))
void on_led_write_cgo(struct avr_t *avr, avr_io_addr_t addr, uint8_t v, void *param) {
	on_led_write(avr, addr, v, param);
}
*/
import "C"

import (
	"unsafe"

	"github.com/mattn/go-pointer"
)

var (
	on_led_write_cgo = C.avr_io_write_t(C.on_led_write_cgo)
	on_adc_irq_cgo   = C.avr_irq_notify_t(C.on_adc_irq_cgo)
)

//export on_adc_irq
func on_adc_irq(irq *C.avr_irq_t, value uint32, param unsafe.Pointer) {
	pointer.Restore(param).(*Sim).on_adc_irq(irq, value, param)
}

//export on_led_write
func on_led_write(avr *C.avr_t, addr C.avr_io_addr_t, v uint8, param unsafe.Pointer) {
	pointer.Restore(param).(*Sim).on_led_write(avr, addr, v, param)
}
