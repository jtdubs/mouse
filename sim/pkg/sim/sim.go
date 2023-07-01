package sim

/*
#cgo pkg-config: simavr simavrparts

#include <avr_adc.h>
#include <avr_ioport.h>
#include <avr_timer.h>
#include <parts/uart_pty.h>
#include <sim_vcd_file.h>
#include <sim_avr.h>
#include <sim_io.h>
#include <sim_elf.h>
#include <sim_gdb.h>
*/
import "C"

import (
	"flag"
	"log"
	"reflect"
	"unsafe"
)

var (
	firmwarePath = flag.String("firmware", "", "Firmware to load")
	gdbEnabled   = flag.Bool("gdb", false, "Enable GDB")
)

type State int

const (
	Paused State = iota
	Running
	Crashed
	Done
)

type Sim struct {
	State          State
	Recording      bool
	RAM            []byte
	Symbols        *SymbolManager
	loopShouldExit bool
	loopChan       chan struct{}
	avr            *C.avr_t
	pty            C.uart_pty_t
	Mouse          *Mouse
	Maze           *Maze
}

func New() *Sim {
	return &Sim{
		State:     Paused,
		Recording: false,
		Maze:      newMaze(),
		Symbols:   NewSymbolManager(),
	}
}

func (s *Sim) Micros() uint64 {
	return (uint64(s.avr.cycle) >> 4)
}

func (s *Sim) Nanos() uint64 {
	return (uint64(s.avr.cycle) * 62) + (uint64(s.avr.cycle) >> 1)
}

func (s *Sim) SetRunning(running bool) {
	if running && s.State == Paused {
		go s.loop(0, func() bool { return false })
	} else if !running && s.State == Running {
		s.loopShouldExit = true
		<-s.loopChan
		s.loopShouldExit = false
	}
}

func (s *Sim) Step(nearestNanos uint64) {
	if s.State == Paused {
		nanos := s.Nanos()
		target := nanos - (nanos % nearestNanos) + nearestNanos
		go s.loop(target, func() bool { return false })
	}
}

func (s *Sim) StepUntil(untilFn func() bool) {
	if s.State == Paused {
		go s.loop(0, untilFn)
	}
}

func (s *Sim) SetRecording(recording bool) {
	wasRunning := s.State == Running
	s.SetRunning(false)
	defer s.SetRunning(wasRunning)

	if recording && !s.Recording {
		C.avr_vcd_start(s.avr.vcd)
		s.Recording = true
	} else if !recording && s.Recording {
		C.avr_vcd_stop(s.avr.vcd)
		s.Recording = false
	}
}

func (s *Sim) Reset() {
	if s.State == Crashed {
		return
	}

	wasRunning := s.State == Running
	s.SetRunning(false)
	C.avr_reset(s.avr)
	s.SetRunning(wasRunning)
}

func (s *Sim) Init() {
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

	s.Symbols.Init(s, *firmwarePath)

	// Point RAM to the AVR's RAM.
	hdr := (*reflect.SliceHeader)(unsafe.Pointer(&s.RAM))
	hdr.Data = uintptr(unsafe.Pointer(s.avr.data))
	hdr.Len = int(s.avr.ramend)
	hdr.Cap = int(s.avr.ramend)

	s.Mouse = NewMouse(s.avr, s.Maze)
	s.Mouse.Init()

	if *gdbEnabled {
		s.avr.state = C.cpu_Stopped
		s.avr.gdb_port = 1234
		if C.avr_gdb_init(s.avr) != 0 {
			log.Fatalf("Failed to initialize GDB")
		}
	}

	C.uart_pty_init(s.avr, &s.pty)
	C.uart_pty_connect(&s.pty, '0')
}

func (s *Sim) loop(until uint64, breakpointFn func() bool) {
	s.loopChan = make(chan struct{})
	defer close(s.loopChan)

	s.State = Running

	var state int = C.cpu_Running
	for !s.loopShouldExit && (until == 0 || s.Nanos() <= until) && !breakpointFn() {
		state = int(C.avr_run(s.avr))
		if state == C.cpu_Done {
			s.State = Done
			return
		} else if state == C.cpu_Crashed {
			log.Printf("AVR crashed")
			s.State = Crashed
			return
		}
	}

	s.State = Paused
}
