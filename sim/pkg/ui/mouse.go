package ui

import (
	"fmt"
	"math"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/sim/pkg/sim"
)

type mouseWindow struct{}

func newMouseWindow() *mouseWindow {
	return &mouseWindow{}
}

func (s *mouseWindow) draw(m *sim.Mouse) {
	imgui.Begin("Mouse Sim")

	imgui.SeparatorText("Controls")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	s.drawControls(m)
	imgui.EndGroup()

	imgui.SeparatorText("Status")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	s.drawStatus(m)
	imgui.EndGroup()

	imgui.End()
}

func (s *mouseWindow) drawControls(m *sim.Mouse) {
	imgui.BeginTable("##Controls", 2)
	imgui.TableSetupColumnV("##ControlsLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##ControlsControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	// Battery Voltage
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Voltage: ")
	imgui.TableSetColumnIndex(1)
	imgui.SliderInt("(V)##BATTERY", &m.Battery.Voltage, 0, 9000)

	// Function Select
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Function: ")
	imgui.TableSetColumnIndex(1)
	functions := "0\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015"
	imgui.ComboStr("##FSEL", &m.FunctionSelector.Function, functions)
	imgui.SameLineV(0, 20)
	imgui.Checkbox("Button", &m.FunctionSelector.ButtonPressed)

	// Left Sensor
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Left Sensor: ")
	imgui.TableSetColumnIndex(1)
	imgui.SliderInt("(V)##LEFT", &m.LeftSensor.Voltage, 0, 5000)

	// Center Sensor
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Center Sensor: ")
	imgui.TableSetColumnIndex(1)
	imgui.SliderInt("(V)##CENTER", &m.CenterSensor.Voltage, 0, 5000)

	// Right Sensor
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Right Sensor: ")
	imgui.TableSetColumnIndex(1)
	imgui.SliderInt("(V)##RIGHT", &m.RightSensor.Voltage, 0, 5000)

	imgui.EndTable()
}

func (s *mouseWindow) drawStatus(m *sim.Mouse) {
	imgui.BeginTable("##Status", 2)
	imgui.TableSetupColumnV("##StatusLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##StatusControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	// LED
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Onboard LED: ")
	imgui.TableSetColumnIndex(1)
	if m.LEDs.Onboard {
		imgui.Text("On")
	} else {
		imgui.Text("Off")
	}

	// Left LED
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Left LED: ")
	imgui.TableSetColumnIndex(1)
	if m.LEDs.Left {
		imgui.Text("On")
	} else {
		imgui.Text("Off")
	}

	// Right LED
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Right LED: ")
	imgui.TableSetColumnIndex(1)
	if m.LEDs.Right {
		imgui.Text("On")
	} else {
		imgui.Text("Off")
	}

	// IR LEDs
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("IR LEDs: ")
	imgui.TableSetColumnIndex(1)
	if m.LEDs.IR {
		imgui.Text("On")
	} else {
		imgui.Text("Off")
	}

	// Left Motor
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Left Motor: ")
	imgui.TableSetColumnIndex(1)
	imgui.Text(fmt.Sprintf("%2.2f (Hz)", 16000000.0/float32(m.LeftMotor.ActualPeriod*240)))

	// Right Motor
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Right Motor: ")
	imgui.TableSetColumnIndex(1)
	imgui.Text(fmt.Sprintf("%2.2f (Hz)", 16000000.0/float32(m.RightMotor.ActualPeriod*240)))

	// Location
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Location: ")
	imgui.TableSetColumnIndex(1)
	imgui.Text(fmt.Sprintf("x=%2.2f, y=%2.2f, theta=%2.2f", m.Environment.MouseX, m.Environment.MouseY, m.Environment.MouseAngle*180.0/math.Pi))

	imgui.EndTable()
}
