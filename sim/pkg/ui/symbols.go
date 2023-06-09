package ui

import (
	"bytes"
	"fmt"
	"math"
	"sort"
	"strings"

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
	w.names = maps.Keys(w.sim.Symbols.Symbols)
	sort.Strings(w.names)
}

func (w *symbolsWindow) draw() {
	if !imgui.Begin("Symbols") {
		return
	}

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
	imgui.TableSetupColumnV("Variable##SymbolsLabel", imgui.TableColumnFlagsWidthFixed, 280, 0)
	imgui.TableSetupColumnV("Value##SymbolsControl", imgui.TableColumnFlagsWidthStretch, 0, 0)
	imgui.TableHeadersRow()

	for _, name := range w.names {
		if !w.filter.PassFilter(name) {
			continue
		}
		imgui.TableNextRow()
		imgui.TableSetColumnIndex(0)
		sym := w.sim.Symbols.Symbols[name]
		switch sym.Length {
		case 1:
			imgui.Text(name)
			imgui.TableSetColumnIndex(1)
			if w.hex {
				imgui.Text(fmt.Sprintf("0x%02x", sym.ReadU8()))
			} else {
				imgui.Text(fmt.Sprintf("%d", sym.ReadU8()))
			}
		case 2:
			imgui.Text(name)
			imgui.TableSetColumnIndex(1)
			if w.hex {
				imgui.Text(fmt.Sprintf("0x%04x", sym.ReadU16()))
			} else {
				imgui.Text(fmt.Sprintf("%d", sym.ReadU16()))
			}
		case 4:
			imgui.Text(name)
			imgui.TableSetColumnIndex(1)
			if strings.Contains(sym.Name, "theta") {
				imgui.Text(fmt.Sprintf("%8.2f°", sym.ReadF32()*180.0/math.Pi))
			} else if strings.HasPrefix(sym.Name, "speed_") || strings.HasPrefix(sym.Name, "position_") || strings.HasPrefix(sym.Name, "linear_") || strings.HasPrefix(sym.Name, "explore_cell_offset") {
				imgui.Text(fmt.Sprintf("%8.2f", sym.ReadF32()))
			} else {
				if w.hex {
					imgui.Text(fmt.Sprintf("0x%08x", sym.ReadU32()))
				} else {
					imgui.Text(fmt.Sprintf("%d", sym.ReadU32()))
				}
			}
		default:
			imgui.Text(fmt.Sprintf("%v[]", name))
			imgui.TableSetColumnIndex(1)
			data := sym.Read()
			var buffer bytes.Buffer
			for i := 0; i < len(data); i++ {
				if i%8 == 0 && i != 0 {
					fmt.Fprintf(&buffer, "\n")
				}
				if i%8 == 0 {
					fmt.Fprintf(&buffer, "%04x:", i)
				}
				fmt.Fprintf(&buffer, " %02x", data[i])
			}
			fmt.Fprintf(&buffer, "\n")
			imgui.Text(buffer.String())
		}
	}

	imgui.PopStyleVar()

	imgui.EndTable()
	imgui.End()
}
