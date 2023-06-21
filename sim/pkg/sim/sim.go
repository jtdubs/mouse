package sim

/*
#cgo CFLAGS: -I/usr/local/include/simavr
#cgo CFLAGS: -I/usr/local/include/simavr/parts
#cgo LDFLAGS: -L/usr/local/lib -lsimavr -lsimavrparts -lelf

#include <avr_adc.h>
#include <avr_ioport.h>
#include <avr_timer.h>
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
	"log"
	"os"
	"os/signal"
	"syscall"
)

var (
	firmwarePath = flag.String("firmware", "", "Firmware to load")
	gdbEnabled   = flag.Bool("gdb", false, "Enable GDB")
)

type Sim struct {
	avr                                   *C.avr_t
	pty                                   C.uart_pty_t
	LEDs                                  *LEDs
	Battery                               *Battery
	FunctionSelector                      *FunctionSelector
	LeftSensor, CenterSensor, RightSensor *Sensor
	LeftMotor, RightMotor                 *Motor
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

	s.LEDs = NewLEDs(s.avr)
	s.Battery = NewBattery(s.avr, C.ADC_IRQ_ADC7)
	s.FunctionSelector = NewFunctionSelect(s.avr, C.ADC_IRQ_ADC6)
	s.LeftMotor = NewMotor(s.avr, true)
	s.RightMotor = NewMotor(s.avr, false)
	s.LeftSensor = NewSensor(s.avr, "SENSOR_LEFT", C.ADC_IRQ_ADC2)
	s.CenterSensor = NewSensor(s.avr, "SENSOR_CENTER", C.ADC_IRQ_ADC1)
	s.RightSensor = NewSensor(s.avr, "SENSOR_RIGHT", C.ADC_IRQ_ADC0)

	s.LEDs.Init()
	s.Battery.Init()
	s.FunctionSelector.Init()
	s.LeftMotor.Init()
	s.RightMotor.Init()
	s.LeftSensor.Init()
	s.CenterSensor.Init()
	s.RightSensor.Init()

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
