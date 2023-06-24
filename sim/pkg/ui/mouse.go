package ui

import (
	"fmt"
	"math"
	"path/filepath"
	"strings"

	"github.com/jtdubs/mouse/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

func mustLoad(image string) *imgui.Texture {
	img, err := imgui.LoadImage(image)
	if err != nil {
		panic(err)
	}
	tex := imgui.NewTextureFromRgba(img)
	fmt.Printf("Loaded %s (%vx%v) as %d\n", image, tex.Width, tex.Height, tex.ID())
	return tex
}

type mouseWindow struct {
	sim      *sim.Sim
	m        *sim.Mouse
	textures map[string]*imgui.Texture
}

func newMouseWindow(sim *sim.Sim) *mouseWindow {
	return &mouseWindow{
		sim:      sim,
		m:        sim.Mouse,
		textures: map[string]*imgui.Texture{},
	}
}

func (s *mouseWindow) init() {
	files, _ := filepath.Glob("icons/*.png")
	for _, file := range files {
		name := strings.TrimSuffix(filepath.Base(file), filepath.Ext(file))
		s.textures[name] = mustLoad(file)
	}
}

func (s *mouseWindow) toolbarButton(name string, icon string) bool {
	return imgui.ImageButtonV(name, s.textures[icon].ID(), imgui.NewVec2(32, 32), imgui.NewVec2(0, 0), imgui.NewVec2(1, 1), imgui.NewVec4(0, 0, 0, 0), imgui.NewVec4(1, 1, 1, 1))
}

func (s *mouseWindow) drawToolbar() {
	switch s.sim.State {
	case sim.Crashed:
		imgui.Text("Crashed")
	case sim.Done:
		imgui.Text("Done")
	default:
		imgui.BeginDisabledV(s.sim.State == sim.Running)
		if s.toolbarButton("SimPlay", "play") {
			s.sim.SetRunning(true)
		}
		imgui.EndDisabled()
		imgui.SameLine()
		imgui.BeginDisabledV(s.sim.State == sim.Paused)
		if s.toolbarButton("SimPause", "pause") {
			s.sim.SetRunning(false)
		}
		imgui.EndDisabled()
	}
	imgui.SameLineV(0, 20)

	imgui.BeginDisabledV(s.sim.Recording)
	if s.toolbarButton("VCDRecord", "video-outline") {
		s.sim.SetRecording(true)
	}
	imgui.EndDisabled()
	imgui.SameLine()
	imgui.BeginDisabledV(!s.sim.Recording)
	if s.toolbarButton("VCDStop", "video-off-outline") {
		s.sim.SetRecording(false)
	}
	imgui.EndDisabled()
}

func (s *mouseWindow) draw() {
	imgui.Begin("Mouse Sim")

	imgui.SeparatorText("Toolbar")
	s.drawToolbar()

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
