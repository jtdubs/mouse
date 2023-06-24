package ui

import (
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

	for _, name := range s.names {
		imgui.Text(name)
	}

	imgui.End()
}
