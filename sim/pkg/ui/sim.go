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

func (s *simWindow) drawControls(sim *sim.Sim) {
	imgui.BeginTable("##Controls", 2)
	imgui.TableSetupColumnV("##ControlsLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##ControlsControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	// Voltage
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Voltage: ")
	imgui.TableSetColumnIndex(1)
	imgui.SliderInt("(V)", &sim.Battery.Voltage, 0, 9000)

	// Function Select
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Function: ")
	imgui.TableSetColumnIndex(1)
	functions := "0\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015"
	imgui.ComboStr("##FSEL", &sim.FunctionSelector.Function, functions)
	imgui.SameLineV(0, 20)
	imgui.Checkbox("Button", &sim.FunctionSelector.ButtonPressed)

	imgui.EndTable()
}

func (s *simWindow) drawStatus(sim *sim.Sim) {
	imgui.BeginTable("##Status", 2)
	imgui.TableSetupColumnV("##StatusLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##StatusControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	// LED
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("LED: ")
	imgui.TableSetColumnIndex(1)
	if sim.LED.State {
		imgui.Text("On")
	} else {
		imgui.Text("Off")
	}

	// Left Motor
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Left Motor: ")
	imgui.TableSetColumnIndex(1)
	imgui.Text(fmt.Sprintf("%v (Hz)", sim.LeftMotor.ActualFrequency/4))

	// Right Motor
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Right Motor: ")
	imgui.TableSetColumnIndex(1)
	imgui.Text(fmt.Sprintf("%v (Hz)", sim.RightMotor.ActualFrequency/4))

	imgui.EndTable()
}

func (s *simWindow) draw(sim *sim.Sim) {
	imgui.Begin("Mouse Sim")

	imgui.SeparatorText("Controls")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	s.drawControls(sim)
	imgui.EndGroup()

	imgui.SeparatorText("Status")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	s.drawStatus(sim)
	imgui.EndGroup()

	imgui.End()
}
