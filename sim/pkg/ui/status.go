package ui

import (
	"fmt"
	"math"

	"github.com/jtdubs/mouse/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

type statusWindow struct {
	sim *sim.Sim
	m   *sim.Mouse
}

func newStatusWindow(sim *sim.Sim) *statusWindow {
	return &statusWindow{
		sim: sim,
		m:   sim.Mouse,
	}
}

func (s *statusWindow) init() {}

func (s *statusWindow) draw() {
	imgui.Begin("Status")

	imgui.BeginTable("##Status", 2)
	imgui.TableSetupColumnV("##StatusLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##StatusControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	s.drawLed("Onboard LED:", s.m.LEDs.Onboard)
	s.drawLed("Left LED:", s.m.LEDs.Left)
	s.drawLed("Right LED:", s.m.LEDs.Right)
	s.drawLed("IR LEDs:", s.m.LEDs.IR)

	s.tableRow("Left Motor:")
	imgui.Text(fmt.Sprintf("%2.2f (Hz)", 16000000.0/float32(s.m.LeftMotor.ActualPeriod*240)))

	s.tableRow("Right Motor:")
	imgui.Text(fmt.Sprintf("%2.2f (Hz)", 16000000.0/float32(s.m.RightMotor.ActualPeriod*240)))

	s.tableRow("Location:")
	imgui.Text(fmt.Sprintf("x=%2.2f, y=%2.2f, theta=%2.2f", s.m.Environment.MouseX, s.m.Environment.MouseY, s.m.Environment.MouseAngle*180.0/math.Pi))

	imgui.EndTable()
	imgui.End()
}

func (s *statusWindow) tableRow(label string) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(label)
	imgui.TableSetColumnIndex(1)
}

func (s *statusWindow) drawLed(label string, value bool) {
	s.tableRow(label)
	if value {
		imgui.ImageV(Textures["led-on"].ID(), imgui.NewVec2(24, 24), imgui.NewVec2(0, 0), imgui.NewVec2(1, 1), imgui.NewVec4(1, 1, 1, 1), imgui.NewVec4(1, 1, 1, 1))
	} else {
		imgui.ImageV(Textures["led-off"].ID(), imgui.NewVec2(24, 24), imgui.NewVec2(0, 0), imgui.NewVec2(1, 1), imgui.NewVec4(1, 1, 1, 1), imgui.NewVec4(1, 1, 1, 1))
	}
}
