package ui

import (
	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/gosim/pkg/sim"
)

type simWindow struct{}

func newSimWindow() *simWindow {
	return &simWindow{}
}

func (s *simWindow) draw(sim *sim.Sim) {
	imgui.Begin("Mouse Sim")

	// LeftIRSensor
	imgui.Text("Left IR Sensor: ")
	imgui.SameLine()
	v := int32(sim.Voltage)
	imgui.SliderInt("Left IR Sensor", &v, 0, 5000)
	sim.Voltage = uint(v)
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
