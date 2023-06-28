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
	hex    bool
}

func newSymbolsWindow(sim *sim.Sim) *symbolsWindow {
	return &symbolsWindow{
		sim:    sim,
		filter: imgui.NewTextFilter(""),
	}
}

func (w *symbolsWindow) init() {
	w.names = maps.Keys(w.sim.Symbols)
	sort.Strings(w.names)
}

func (w *symbolsWindow) draw() {
	imgui.Begin("Symbols")

	imgui.Text("Filter: ")
	imgui.SameLine()
	w.filter.DrawV("", 180)
	imgui.SameLine()
	imgui.Checkbox("Hex", &w.hex)
	imgui.Separator()

	var tableFlags imgui.TableFlags = imgui.TableFlagsResizable |
		imgui.TableFlagsBorders |
		imgui.TableFlagsBordersInnerV

	imgui.BeginTableV("##Symbols", 2, tableFlags, imgui.NewVec2(0, 0), 0)
	imgui.TableSetupColumnV("Variable##SymbolsLabel", imgui.TableColumnFlagsWidthFixed, 200, 0)
	imgui.TableSetupColumnV("Value##SymbolsControl", imgui.TableColumnFlagsWidthStretch, 0, 0)
	imgui.TableHeadersRow()

	for _, name := range w.names {
		if !w.filter.PassFilter(name) {
			continue
		}
		imgui.TableNextRow()
		imgui.TableSetColumnIndex(0)
		imgui.Text(name)
		imgui.TableSetColumnIndex(1)
		sym := w.sim.Symbols[name]
		switch sym.Length {
		case 1:
			if w.hex {
				imgui.Text(fmt.Sprintf("0x%02x", w.sim.RAM[sym.Address]))
			} else {
				imgui.Text(fmt.Sprintf("%d", w.sim.RAM[sym.Address]))
			}
		case 2:
			if w.hex {
				imgui.Text(fmt.Sprintf("0x%04x", binary.LittleEndian.Uint16(w.sim.RAM[sym.Address:sym.Address+2])))
			} else {
				imgui.Text(fmt.Sprintf("%d", binary.LittleEndian.Uint16(w.sim.RAM[sym.Address:sym.Address+2])))
			}
		case 4:
			if w.hex {
				imgui.Text(fmt.Sprintf("0x%08x", binary.LittleEndian.Uint32(w.sim.RAM[sym.Address:sym.Address+4])))
			} else {
				imgui.Text(fmt.Sprintf("%d", binary.LittleEndian.Uint32(w.sim.RAM[sym.Address:sym.Address+4])))
			}
		default:
			imgui.Text(hex.EncodeToString(w.sim.RAM[sym.Address : sym.Address+sym.Length]))
		}
	}

	imgui.PopStyleVar()

	imgui.EndTable()
	imgui.End()
}
