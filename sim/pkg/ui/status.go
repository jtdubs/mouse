package ui

import (
	"fmt"

	"github.com/jtdubs/mouse/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

type statusWindow struct {
	sim   *sim.Sim
	mouse *sim.Mouse
}

func newStatusWindow(sim *sim.Sim) *statusWindow {
	return &statusWindow{
		sim:   sim,
		mouse: sim.Mouse,
	}
}

func (w *statusWindow) init() {}

func (w *statusWindow) draw() {
	if !imgui.Begin("Status") {
		return
	}

	imgui.BeginTable("##Status", 2)
	imgui.TableSetupColumnV("##StatusLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##StatusControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	w.tableRow("Status LEDs:")
	if w.mouse.LEDs.Left {
		w.drawIcon("led-on-lightblue")
	} else {
		w.drawIcon("led-off-white")
	}
	imgui.SameLineV(0, 20)
	if w.mouse.LEDs.Onboard {
		w.drawIcon("led-on-orange")
	} else {
		w.drawIcon("led-off-white")
	}
	imgui.SameLineV(0, 20)
	if w.mouse.LEDs.Right {
		w.drawIcon("led-on-lightblue")
	} else {
		w.drawIcon("led-off-white")
	}

	w.tableRow("IR LEDs:")
	if w.mouse.LEDs.IR {
		w.drawIcon("led-on-red")
		imgui.SameLineV(0, 20)
		w.drawIcon("led-on-red")
		imgui.SameLineV(0, 20)
		w.drawIcon("led-on-red")
	} else {
		w.drawIcon("led-off-white")
		imgui.SameLineV(0, 20)
		w.drawIcon("led-off-white")
		imgui.SameLineV(0, 20)
		w.drawIcon("led-off-white")
	}

	w.tableRow("Motors:")
	imgui.Text(fmt.Sprintf("%02.2f (Hz)", 16000000.0/float32(w.mouse.LeftMotor.ActualPeriod*240)))
	imgui.SameLineV(0, 20)
	imgui.Text(fmt.Sprintf("%02.2f (Hz)", 16000000.0/float32(w.mouse.RightMotor.ActualPeriod*240)))

	imgui.EndTable()
	imgui.End()
}

func (w *statusWindow) tableRow(label string) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(label)
	imgui.TableSetColumnIndex(1)
}

func (w *statusWindow) drawIcon(name string) {
	imgui.ImageV(Textures[name].ID(), imgui.NewVec2(24, 24), imgui.NewVec2(0, 0), imgui.NewVec2(1, 1), imgui.NewVec4(1, 1, 1, 1), imgui.NewVec4(0, 0, 0, 0))
}
