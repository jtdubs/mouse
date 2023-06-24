package ui

import (
	"fmt"
	"math"

	"github.com/jtdubs/mouse/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

type mouseWindow struct {
	sim *sim.Sim
	m   *sim.Mouse
}

func newMouseWindow(sim *sim.Sim) *mouseWindow {
	return &mouseWindow{
		sim: sim,
		m:   sim.Mouse,
	}
}

func (s *mouseWindow) init() {}

func (s *mouseWindow) draw(dockID imgui.ID) {
	imgui.SetNextWindowDockIDV(dockID, imgui.CondFirstUseEver)

	imgui.Begin("Mouse Sim")

	imgui.SeparatorText("Controls")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	s.drawControls()
	imgui.EndGroup()

	imgui.SeparatorText("Status")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	s.drawStatus()
	imgui.EndGroup()

	imgui.End()
}

func (s *mouseWindow) tableRow(label string) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(label)
	imgui.TableSetColumnIndex(1)
}

func (s *mouseWindow) drawControls() {
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

	s.tableRow("Left Sensor:")
	imgui.SliderInt("(V)##LEFT", &s.m.LeftSensor.Voltage, 0, 5000)

	s.tableRow("Center Sensor:")
	imgui.SliderInt("(V)##CENTER", &s.m.CenterSensor.Voltage, 0, 5000)

	s.tableRow("Right Sensor:")
	imgui.SliderInt("(V)##RIGHT", &s.m.RightSensor.Voltage, 0, 5000)

	imgui.EndTable()
}

func (s *mouseWindow) drawStatus() {
	imgui.BeginTable("##Status", 2)
	imgui.TableSetupColumnV("##StatusLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##StatusControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	s.tableRow("Onboard LED:")
	if s.m.LEDs.Onboard {
		imgui.Text("On")
	} else {
		imgui.Text("Off")
	}

	s.tableRow("Left LED:")
	if s.m.LEDs.Left {
		imgui.Text("On")
	} else {
		imgui.Text("Off")
	}

	s.tableRow("Right LED:")
	if s.m.LEDs.Right {
		imgui.Text("On")
	} else {
		imgui.Text("Off")
	}

	s.tableRow("IR LEDs:")
	if s.m.LEDs.IR {
		imgui.Text("On")
	} else {
		imgui.Text("Off")
	}

	s.tableRow("Left Motor:")
	imgui.Text(fmt.Sprintf("%2.2f (Hz)", 16000000.0/float32(s.m.LeftMotor.ActualPeriod*240)))

	s.tableRow("Right Motor:")
	imgui.Text(fmt.Sprintf("%2.2f (Hz)", 16000000.0/float32(s.m.RightMotor.ActualPeriod*240)))

	s.tableRow("Location:")
	imgui.Text(fmt.Sprintf("x=%2.2f, y=%2.2f, theta=%2.2f", s.m.Environment.MouseX, s.m.Environment.MouseY, s.m.Environment.MouseAngle*180.0/math.Pi))

	imgui.EndTable()
}
