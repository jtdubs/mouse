package ui

import (
	"encoding/binary"
	"encoding/hex"
	"fmt"
	"sort"

	"github.com/jtdubs/mouse/sim/pkg/sim"
	"golang.org/x/exp/maps"

	imgui "github.com/AllenDang/cimgui-go"
)

type symbolsWindow struct {
	sim   *sim.Sim
	names []string
}

func newSymbolsWindow(sim *sim.Sim) *symbolsWindow {
	return &symbolsWindow{
		sim: sim,
	}
}

func (s *symbolsWindow) init() {
	s.names = maps.Keys(s.sim.Symbols)
	sort.Strings(s.names)
}

func (s *symbolsWindow) draw() {
	imgui.Begin("Symbols")

	imgui.BeginTable("##Symbols", 2)
	imgui.TableSetupColumnV("##SymbolsLabel", imgui.TableColumnFlagsWidthFixed, 200, 0)
	imgui.TableSetupColumnV("##SymbolsControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	for _, name := range s.names {
		imgui.TableNextRow()
		imgui.TableSetColumnIndex(0)
		imgui.Text(name)
		imgui.TableSetColumnIndex(1)
		sym := s.sim.Symbols[name]
		switch sym.Length {
		case 1:
			imgui.Text(fmt.Sprintf("0x%02x", s.sim.RAM[sym.Address]))
		case 2:
			imgui.Text(fmt.Sprintf("0x%04x", binary.LittleEndian.Uint16(s.sim.RAM[sym.Address:sym.Address+2])))
		case 4:
			imgui.Text(fmt.Sprintf("0x%08x", binary.LittleEndian.Uint32(s.sim.RAM[sym.Address:sym.Address+4])))
		default:
			imgui.Text(hex.EncodeToString(s.sim.RAM[sym.Address : sym.Address+sym.Length]))
		}
	}

	imgui.EndTable()
	imgui.End()
}
