package sim

/*
#cgo CFLAGS: -I/usr/include/simavr
#cgo CFLAGS: -I/usr/include/simavr/parts
#cgo CFLAGS: -I.
#cgo LDFLAGS: -L. -lsimavr -lsimavrparts

#include <avr_adc.h>
#include <avr_ioport.h>
#include <parts/uart_pty.h>
#include <sim_vcd_file.h>
#include <sim_avr.h>
#include <sim_elf.h>
#include <sim_gdb.h>
*/
import "C"

import (
	"context"
	"flag"
	"fmt"
	"log"
	"os"
	"os/signal"
	"syscall"
	"unsafe"

	"github.com/mattn/go-pointer"
)

var (
	firmwarePath = flag.String("firmware", "", "Firmware to load")
	gdbEnabled   = flag.Bool("gdb", false, "Enable GDB")
)

type Sim struct {
	avr                  *C.avr_t
	pty                  C.uart_pty_t
	LED                  bool
	Voltage              uint
	FunctionSelect       [4]bool
	FunctionSelectButton bool
}

func New() *Sim {
	return &Sim{}
}

func (s *Sim) Run(ctx context.Context) {
	if *firmwarePath == "" {
		log.Fatalf("No firmware specified")
	}

	var f C.elf_firmware_t
	if C.elf_read_firmware(C.CString(*firmwarePath), &f) != 0 {
		log.Fatalf("Failed to read firmware: %s", *firmwarePath)
	}

	s.avr = C.avr_make_mcu_by_name(&f.mmcu[0])
	if s.avr == nil {
		log.Fatalf("Failed to create AVR")
	}

	if C.avr_init(s.avr) != 0 {
		log.Fatalf("Failed to initialize AVR")
	}

	C.avr_load_firmware(s.avr, &f)

	C.avr_vcd_add_signal(s.avr.vcd, C.avr_io_getirq(s.avr, C.AVR_IOCTL_ADC_GETIRQ, C.ADC_IRQ_ADC0), 16, C.CString("SENSOR0"))
	C.avr_vcd_add_signal(s.avr.vcd, C.avr_io_getirq(s.avr, C.AVR_IOCTL_ADC_GETIRQ, C.ADC_IRQ_ADC1), 16, C.CString("SENSOR1"))
	C.avr_vcd_add_signal(s.avr.vcd, C.avr_io_getirq(s.avr, C.AVR_IOCTL_ADC_GETIRQ, C.ADC_IRQ_ADC2), 16, C.CString("SENSOR2"))
	C.avr_vcd_add_signal(s.avr.vcd, C.avr_io_getirq(s.avr, C.AVR_IOCTL_ADC_GETIRQ, C.ADC_IRQ_ADC6), 16, C.CString("FUNCTION_SELECT"))
	C.avr_vcd_add_signal(s.avr.vcd, C.avr_io_getirq(s.avr, C.AVR_IOCTL_ADC_GETIRQ, C.ADC_IRQ_ADC7), 16, C.CString("BATTERY_VOLTAGE"))

	C.avr_irq_register_notify(C.avr_io_getirq(s.avr, C.AVR_IOCTL_ADC_GETIRQ, C.ADC_IRQ_OUT_TRIGGER), on_adc_irq_cgo, pointer.Save(s))
	C.avr_register_io_write(s.avr, 0x25, on_led_write_cgo, pointer.Save(s))

	if *gdbEnabled {
		s.avr.state = C.cpu_Stopped
		s.avr.gdb_port = 1234
		if C.avr_gdb_init(s.avr) != 0 {
			log.Fatalf("Failed to initialize GDB")
		}
	}

	C.uart_pty_init(s.avr, &s.pty)
	C.uart_pty_connect(&s.pty, '0')

	signals := make(chan os.Signal, 1)
	signal.Notify(signals, syscall.SIGINT, syscall.SIGTERM)

	var exit bool
	go func() {
		var state int = C.cpu_Running
		for !exit {
			state = int(C.avr_run(s.avr))
			if state == C.cpu_Done {
				break
			} else if state == C.cpu_Crashed {
				log.Fatalf("AVR crashed")
			}
		}
	}()

	select {
	case <-signals:
	case <-ctx.Done():
	}
	exit = true
}

// Values of each function in millivolts.
var functionVoltages = [16]int{
	3235, 3176, 3098, 3019, 2882, 2784, 2666, 2549, 2274, 2117, 1901, 1686, 1333, 1039, 627, 215,
}

func (s *Sim) on_adc_irq(irq *C.avr_irq_t, value uint32, param unsafe.Pointer) {
	C.avr_raise_irq(C.avr_io_getirq(s.avr, C.AVR_IOCTL_ADC_GETIRQ, C.ADC_IRQ_ADC7), C.uint(s.Voltage/2))

	if s.FunctionSelectButton {
		C.avr_raise_irq(C.avr_io_getirq(s.avr, C.AVR_IOCTL_ADC_GETIRQ, C.ADC_IRQ_ADC6), C.uint(5000))
	} else {
		x := 0
		for i, v := range s.FunctionSelect {
			if v {
				x |= 1 << uint(i)
			}
		}
		C.avr_raise_irq(C.avr_io_getirq(s.avr, C.AVR_IOCTL_ADC_GETIRQ, C.ADC_IRQ_ADC6), C.uint(functionVoltages[x]))
	}
}

func (s *Sim) on_led_write(avr *C.avr_t, addr C.avr_io_addr_t, v uint8, param unsafe.Pointer) {
	fmt.Printf("LED Write: %v\n", (v>>5)&1)
	s.LED = (v>>5)&1 == 1
}
