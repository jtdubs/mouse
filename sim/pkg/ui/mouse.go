package ui

import (
	"fmt"
	"math"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/sim/pkg/sim"
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

func (s *mouseWindow) draw() {
	imgui.Begin("Mouse Sim")

	imgui.SeparatorText("Sim")
	imgui.Text("Sim State: ")
	imgui.SameLine()
	switch s.sim.State {
	case sim.Paused:
		if imgui.Button("Paused##Sim") {
			s.sim.SetRunning(true)
		}
	case sim.Running:
		if imgui.Button("Running##Sim") {
			s.sim.SetRunning(false)
		}
	case sim.Crashed:
		imgui.Text("Crashed")
	case sim.Done:
		imgui.Text("Done")
	}
	imgui.Text("VCD: ")
	imgui.SameLine()
	switch s.sim.Recording {
	case true:
		if imgui.Button("Capturing##Recording") {
			s.sim.SetRecording(false)
		}
	case false:
		if imgui.Button("Stopped##Recording") {
			s.sim.SetRecording(true)
		}
	}

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

func (s *mouseWindow) drawControls() {
	imgui.BeginTable("##Controls", 2)
	imgui.TableSetupColumnV("##ControlsLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##ControlsControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	// Battery Voltage
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Voltage: ")
	imgui.TableSetColumnIndex(1)
	imgui.SliderInt("(V)##BATTERY", &s.m.Battery.Voltage, 0, 9000)

	// Function Select
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Function: ")
	imgui.TableSetColumnIndex(1)
	functions := "0\0001\0002\0003\0004\0005\0006\0007\0008\0009\00010\00011\00012\00013\00014\00015"
	imgui.ComboStr("##FSEL", &s.m.FunctionSelector.Function, functions)
	imgui.SameLineV(0, 20)
	imgui.Checkbox("Button", &s.m.FunctionSelector.ButtonPressed)

	// Left Sensor
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Left Sensor: ")
	imgui.TableSetColumnIndex(1)
	imgui.SliderInt("(V)##LEFT", &s.m.LeftSensor.Voltage, 0, 5000)

	// Center Sensor
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Center Sensor: ")
	imgui.TableSetColumnIndex(1)
	imgui.SliderInt("(V)##CENTER", &s.m.CenterSensor.Voltage, 0, 5000)

	// Right Sensor
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Right Sensor: ")
	imgui.TableSetColumnIndex(1)
	imgui.SliderInt("(V)##RIGHT", &s.m.RightSensor.Voltage, 0, 5000)

	imgui.EndTable()
}

func (s *mouseWindow) drawStatus() {
	imgui.BeginTable("##Status", 2)
	imgui.TableSetupColumnV("##StatusLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##StatusControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	// LED
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Onboard LED: ")
	imgui.TableSetColumnIndex(1)
	if s.m.LEDs.Onboard {
		imgui.Text("On")
	} else {
		imgui.Text("Off")
	}

	// Left LED
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Left LED: ")
	imgui.TableSetColumnIndex(1)
	if s.m.LEDs.Left {
		imgui.Text("On")
	} else {
		imgui.Text("Off")
	}

	// Right LED
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Right LED: ")
	imgui.TableSetColumnIndex(1)
	if s.m.LEDs.Right {
		imgui.Text("On")
	} else {
		imgui.Text("Off")
	}

	// IR LEDs
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("IR LEDs: ")
	imgui.TableSetColumnIndex(1)
	if s.m.LEDs.IR {
		imgui.Text("On")
	} else {
		imgui.Text("Off")
	}

	// Left Motor
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Left Motor: ")
	imgui.TableSetColumnIndex(1)
	imgui.Text(fmt.Sprintf("%2.2f (Hz)", 16000000.0/float32(s.m.LeftMotor.ActualPeriod*240)))

	// Right Motor
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Right Motor: ")
	imgui.TableSetColumnIndex(1)
	imgui.Text(fmt.Sprintf("%2.2f (Hz)", 16000000.0/float32(s.m.RightMotor.ActualPeriod*240)))

	// Location
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Location: ")
	imgui.TableSetColumnIndex(1)
	imgui.Text(fmt.Sprintf("x=%2.2f, y=%2.2f, theta=%2.2f", s.m.Environment.MouseX, s.m.Environment.MouseY, s.m.Environment.MouseAngle*180.0/math.Pi))

	imgui.EndTable()
}
