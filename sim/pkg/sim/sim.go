package sim

/*
#cgo pkg-config: simavr simavrparts

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
	"debug/elf"
	"flag"
	"fmt"
	"log"
	"strings"
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
	loopShouldExit bool
	loopChan       chan struct{}
	avr            *C.avr_t
	pty            C.uart_pty_t
	Mouse          *Mouse
}

func New() *Sim {
	return &Sim{
		State:     Paused,
		Recording: false,
	}
}

func (s *Sim) Peek(addr uint16) uint32 {
	return uint32(*(*uint8)(unsafe.Pointer(uintptr(unsafe.Pointer(s.avr.data)) + uintptr(addr))))
}

func (s *Sim) SetRunning(running bool) {
	if running && s.State == Paused {
		go s.loop()
	} else if !running && s.State == Running {
		s.loopShouldExit = true
		<-s.loopChan
		s.loopShouldExit = false
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

	elfFile, err := elf.Open(*firmwarePath)
	if err != nil {
		log.Fatalf("Failed to open firmware: %s", *firmwarePath)
	}
	syms, err := elfFile.Symbols()
	if err != nil {
		log.Fatalf("Failed to read symbols from firmware: %s", *firmwarePath)
	}
	for _, sym := range syms {
		if strings.HasPrefix(sym.Name, "_") {
			continue
		}
		if int(sym.Section) >= len(elfFile.Sections) {
			continue
		}
		section := elfFile.Sections[sym.Section]
		if section.Name != ".bss" {
			continue
		}
		if sym.Name == "" {
			continue
		}
		fmt.Printf("Symbol: %v @ 0x%08x+0x%02x\n", sym.Name, sym.Value-0x800100, sym.Size)
	}

	var f C.elf_firmware_t
	if C.elf_read_firmware(C.CString(*firmwarePath), &f) != 0 {
		log.Fatalf("Failed to read firmware: %s", *firmwarePath)
	}

	s.avr = C.avr_make_mcu_by_name(&f.mmcu[0])
	if s.avr == nil {
		log.Fatalf("Failed to create AVR")
	}

	fmt.Printf("Ramend: %08x", s.avr.ramend)
	// s.avr.data[0x00800135]

	if C.avr_init(s.avr) != 0 {
		log.Fatalf("Failed to initialize AVR")
	}

	C.avr_load_firmware(s.avr, &f)

	s.Mouse = NewMouse(s.avr)
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

func (s *Sim) loop() {
	s.loopChan = make(chan struct{})
	defer close(s.loopChan)

	s.State = Running

	var state int = C.cpu_Running
	for !s.loopShouldExit {
		state = int(C.avr_run(s.avr))
		if state == C.cpu_Done {
			s.State = Done
			return
		} else if state == C.cpu_Crashed {
			log.Fatalf("AVR crashed")
			s.State = Crashed
			return
		}
	}

	s.State = Paused
}
