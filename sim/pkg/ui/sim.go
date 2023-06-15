package ui

import (
	"fmt"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/sim/pkg/sim"
)

type simWindow struct{}

func newSimWindow() *simWindow {
	return &simWindow{}
}

func (s *simWindow) draw(sim *sim.Sim) {
	imgui.Begin("Mouse Sim")

	// Voltage
	imgui.Text("Voltage: ")
	imgui.SameLine()
	v := int32(sim.Voltage)
	imgui.SliderInt("(V)", &v, 0, 9000)
	sim.Voltage = uint(v)
	imgui.Separator()

	// Function Select
	imgui.Text("Function Select: ")
	for i := range sim.FunctionSelect {
		imgui.SameLine()
		imgui.Checkbox(fmt.Sprintf("##FSEL%v", i), &sim.FunctionSelect[3-i])
	}
	imgui.SameLineV(0, 20)
	imgui.Checkbox("Button", &sim.FunctionSelectButton)
	imgui.Separator()

	// LED
	imgui.Text("LED: ")
	imgui.SameLine()
	if sim.LED {
		imgui.Text("On")
	} else {
		imgui.Text("Off")
	}

	imgui.Separator()

	imgui.End()
}
