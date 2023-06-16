package ui

import (
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
	imgui.SliderInt("(V)", &sim.Battery.Voltage, 0, 9000)
	imgui.Separator()

	// Function Select
	imgui.Text("Function Select: ")
	functions := "0\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015"
	imgui.ComboStr("##FSEL", &sim.FunctionSelector.Function, functions)
	imgui.SameLineV(0, 20)
	imgui.Checkbox("Button", &sim.FunctionSelector.ButtonPressed)
	imgui.Separator()

	// LED
	imgui.Text("LED: ")
	imgui.SameLine()
	if sim.LED.State {
		imgui.Text("On")
	} else {
		imgui.Text("Off")
	}
	imgui.Separator()

	imgui.End()
}
