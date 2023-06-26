package ui

import (
	"github.com/jtdubs/mouse/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

type controlsWindow struct {
	sim *sim.Sim
	m   *sim.Mouse
}

func newControlsWindow(sim *sim.Sim) *controlsWindow {
	return &controlsWindow{
		sim: sim,
		m:   sim.Mouse,
	}
}

func (s *controlsWindow) init() {}

func (s *controlsWindow) draw() {
	imgui.Begin("Controls")

	imgui.BeginTable("##Controls", 2)
	imgui.TableSetupColumnV("##ControlsLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##ControlsControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	s.tableRow("Voltage:")
	imgui.SliderInt("(V)##BATTERY", &s.m.Battery.Voltage, 0, 9000)

	s.tableRow("Function:")
	functions := "0\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015"
	imgui.ComboStr("##FSEL", &s.m.FunctionSelector.Function, functions)
	imgui.SameLineV(0, 20)
	imgui.Checkbox("Button", &s.m.FunctionSelector.ButtonPressed)

	s.tableRow("IR Sensors:")
	voltages := [3]int32{s.m.LeftSensor.Voltage, s.m.CenterSensor.Voltage, s.m.RightSensor.Voltage}
	imgui.SliderInt3("(mV)##SENSORS", &voltages, 0, 5000)
	s.m.LeftSensor.Voltage = voltages[0]
	s.m.CenterSensor.Voltage = voltages[1]
	s.m.RightSensor.Voltage = voltages[2]

	imgui.EndTable()

	imgui.End()
}

func (s *controlsWindow) tableRow(label string) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(label)
	imgui.TableSetColumnIndex(1)
}