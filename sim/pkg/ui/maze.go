package ui

import (
	"github.com/jtdubs/mouse/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

type mazeWindow struct {
	sim *sim.Sim
	m   *sim.Mouse
}

func newMazeWindow(sim *sim.Sim) *mazeWindow {
	return &mazeWindow{
		sim: sim,
		m:   sim.Mouse,
	}
}

func (s *mazeWindow) init() {}

func (s *mazeWindow) draw() {
	imgui.Begin("Maze")
	imgui.End()
}
