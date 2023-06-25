package ui

import (
	"fmt"

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

	s.tableRow("Status LEDs:")
	if s.m.LEDs.Left {
		s.drawIcon("led-on-lightblue")
	} else {
		s.drawIcon("led-off-white")
	}
	imgui.SameLineV(0, 20)
	if s.m.LEDs.Onboard {
		s.drawIcon("led-on-orange")
	} else {
		s.drawIcon("led-off-white")
	}
	imgui.SameLineV(0, 20)
	if s.m.LEDs.Right {
		s.drawIcon("led-on-lightblue")
	} else {
		s.drawIcon("led-off-white")
	}

	s.tableRow("IR LEDs:")
	if s.m.LEDs.IR {
		s.drawIcon("led-on-red")
		imgui.SameLineV(0, 20)
		s.drawIcon("led-on-red")
		imgui.SameLineV(0, 20)
		s.drawIcon("led-on-red")
	} else {
		s.drawIcon("led-off-white")
		imgui.SameLineV(0, 20)
		s.drawIcon("led-off-white")
		imgui.SameLineV(0, 20)
		s.drawIcon("led-off-white")
	}

	s.tableRow("Motors:")
	imgui.Text(fmt.Sprintf("%02.2f (Hz)", 16000000.0/float32(s.m.LeftMotor.ActualPeriod*240)))
	imgui.SameLineV(0, 20)
	imgui.Text(fmt.Sprintf("%02.2f (Hz)", 16000000.0/float32(s.m.RightMotor.ActualPeriod*240)))

	imgui.EndTable()
	imgui.End()
}

func (s *statusWindow) tableRow(label string) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(label)
	imgui.TableSetColumnIndex(1)
}

func (s *statusWindow) drawIcon(name string) {
	imgui.ImageV(Textures[name].ID(), imgui.NewVec2(24, 24), imgui.NewVec2(0, 0), imgui.NewVec2(1, 1), imgui.NewVec4(1, 1, 1, 1), imgui.NewVec4(0, 0, 0, 0))
}
