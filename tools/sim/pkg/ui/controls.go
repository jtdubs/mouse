package ui

import (
	"github.com/jtdubs/mouse/tools/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

type controlsWindow struct {
	sim   *sim.Sim
	mouse *sim.Mouse
}

func newControlsWindow(sim *sim.Sim) *controlsWindow {
	return &controlsWindow{
		sim:   sim,
		mouse: sim.Mouse,
	}
}

func (w *controlsWindow) init() {}

func (w *controlsWindow) draw() {
	if !imgui.Begin("Controls") {
		return
	}

	imgui.BeginTable("##Controls", 2)
	imgui.TableSetupColumnV("##ControlsLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##ControlsControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	w.tableRow("Voltage:")
	imgui.SliderInt("(V)##BATTERY", &w.mouse.Battery.Voltage, 0, 9000)

	w.tableRow("Function:")
	functions := "0\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015"
	imgui.ComboStr("##FSEL", &w.mouse.FunctionSelector.Function, functions)
	imgui.SameLineV(0, 20)
	imgui.Checkbox("Button", &w.mouse.FunctionSelector.ButtonPressed)

	w.tableRow("IR Sensors:")
	voltages := [3]int32{w.mouse.LeftSensor.Voltage, w.mouse.RightSensor.Voltage, w.mouse.ForwardSensor.Voltage}
	imgui.SliderInt3("(mV)##SENSORS", &voltages, 0, 5000)
	w.mouse.LeftSensor.Voltage = voltages[0]
	w.mouse.RightSensor.Voltage = voltages[1]
	w.mouse.ForwardSensor.Voltage = voltages[2]

	imgui.EndTable()

	imgui.End()
}

func (w *controlsWindow) tableRow(label string) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(label)
	imgui.TableSetColumnIndex(1)
}
