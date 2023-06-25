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

	topLeft := imgui.CursorScreenPos()
	size := imgui.ContentRegionAvail()
	bottomRight := topLeft.Add(size)

	drawList := imgui.WindowDrawList()
	drawList.AddRect(topLeft, bottomRight, imgui.ColorConvertFloat4ToU32(imgui.NewVec4(1, 1, 1, 1)))

	imgui.End()
}
