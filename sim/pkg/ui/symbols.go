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
	sim    *sim.Sim
	names  []string
	filter imgui.TextFilter
}

func newSymbolsWindow(sim *sim.Sim) *symbolsWindow {
	return &symbolsWindow{
		sim:    sim,
		filter: imgui.NewTextFilter(""),
	}
}

func (s *symbolsWindow) init() {
	s.names = maps.Keys(s.sim.Symbols)
	sort.Strings(s.names)
}

func (s *symbolsWindow) draw() {
	imgui.Begin("Symbols")

	imgui.Text("Filter: ")
	imgui.SameLine()
	s.filter.DrawV("", 180)
	imgui.Separator()

	var tableFlags imgui.TableFlags = imgui.TableFlagsResizable |
		imgui.TableFlagsBorders |
		imgui.TableFlagsBordersInnerV

	imgui.BeginTableV("##Symbols", 2, tableFlags, imgui.NewVec2(0, 0), 0)
	imgui.TableSetupColumnV("Variable##SymbolsLabel", imgui.TableColumnFlagsWidthFixed, 200, 0)
	imgui.TableSetupColumnV("Value##SymbolsControl", imgui.TableColumnFlagsWidthStretch, 0, 0)
	imgui.TableHeadersRow()

	for _, name := range s.names {
		if !s.filter.PassFilter(name) {
			continue
		}
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

	imgui.PopStyleVar()

	imgui.EndTable()
	imgui.End()
}
