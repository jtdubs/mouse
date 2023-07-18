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
	"debug/elf"
	"encoding/binary"
	"log"
	"reflect"
	"strings"
	"unsafe"

	"github.com/ianlancetaylor/demangle"
	"github.com/mattn/go-pointer"
)

type Symbol struct {
	Name    string
	Address int
	Length  int
	sim     *Sim
	irq     *C.avr_irq_t
}

func (s *Symbol) Read() []byte {
	return s.sim.RAM[s.Address : s.Address+s.Length]
}

func (s *Symbol) ReadU8() uint8 {
	return s.sim.RAM[s.Address]
}

func (s *Symbol) ReadU16() uint16 {
	return binary.LittleEndian.Uint16(s.sim.RAM[s.Address : s.Address+2])
}

func (s *Symbol) ReadU32() uint32 {
	return binary.LittleEndian.Uint32(s.sim.RAM[s.Address : s.Address+4])
}

func (s *Symbol) ReadF32() float32 {
	u32 := s.ReadU32()
	return *(*float32)(unsafe.Pointer(&u32))
}

func (s *Symbol) ReadVCD() C.uint {
	switch s.Length {
	case 1:
		return C.uint(s.ReadU8())
	case 2:
		return C.uint(s.ReadU16())
	case 4:
		return C.uint(s.ReadU32())
	default:
		return 0
	}
}

type SymbolManager struct {
	sim     *Sim
	Symbols map[string]*Symbol
}

func NewSymbolManager() *SymbolManager {
	return &SymbolManager{
		Symbols: make(map[string]*Symbol),
	}
}

func (s *SymbolManager) Init(sim *Sim, elfPath string) {
	s.sim = sim

	elfFile, err := elf.Open(elfPath)
	if err != nil {
		log.Fatalf("Failed to open firmware: %s", *firmwarePath)
	}

	syms, err := elfFile.Symbols()
	if err != nil {
		log.Fatalf("Failed to read symbols from firmware: %s", *firmwarePath)
	}

	for _, sym := range syms {
		symName := sym.Name
		if strings.HasPrefix(symName, "_") {
			if newName, err := demangle.ToString(sym.Name); err == nil {
				symName = strings.ReplaceAll(newName, "::(anonymous namespace)", "")
			}
		}
		if int(sym.Section) >= len(elfFile.Sections) {
			continue
		}
		section := elfFile.Sections[sym.Section]
		if section.Name != ".bss" {
			continue
		}
		if symName == "" {
			continue
		}

		symbol := &Symbol{
			sim:     sim,
			Name:    symName,
			Address: int(sym.Value - 0x800000),
			Length:  int(sym.Size),
		}

		s.Symbols[symName] = symbol
	}

	var names []*C.char
	for name := range s.Symbols {
		names = append(names, C.CString(name))
	}

	var irqs []C.avr_irq_t
	hdr := (*reflect.SliceHeader)(unsafe.Pointer(&irqs))
	hdr.Data = uintptr(unsafe.Pointer(C.avr_alloc_irq(&sim.avr.irq_pool, 0, C.uint(len(s.Symbols)), &names[0])))
	hdr.Len = len(s.Symbols)
	hdr.Cap = len(s.Symbols)

	i := 0
	for name, sym := range s.Symbols {
		sym.irq = &irqs[i]
		i += 1
		if sym.Length <= 4 {
			C.avr_vcd_add_signal(sim.avr.vcd, sym.irq, C.int(sym.Length*8), C.CString(name))
		}
	}

	tickIRQ := C.avr_io_getirq(sim.avr, 0x696F6743 /* iogC */, C.int(3))
	C.avr_irq_register_notify(tickIRQ, on_irq_cgo, pointer.Save(s))
}

func (s *SymbolManager) OnIRQ(irq *C.avr_irq_t, value uint32, param unsafe.Pointer) {
	for _, sym := range s.Symbols {
		C.avr_raise_irq(sym.irq, sym.ReadVCD())
	}
}
