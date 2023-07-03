package ui

import (
	"fmt"

	"github.com/jtdubs/mouse/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

type toolbarWindow struct {
	sim *sim.Sim
}

func newToolbarWindow(sim *sim.Sim) *toolbarWindow {
	return &toolbarWindow{
		sim: sim,
	}
}

func (w *toolbarWindow) init() {}

func (w *toolbarWindow) toolbarButton(name string, icon string) bool {
	return imgui.ImageButtonV(name, Textures[icon].ID(), imgui.NewVec2(24, 24), imgui.NewVec2(0, 0), imgui.NewVec2(1, 1), imgui.NewVec4(0, 0, 0, 0), imgui.NewVec4(1, 1, 1, 1))
}

func (w *toolbarWindow) drawToolbar() {
	switch w.sim.State {
	case sim.Crashed:
		imgui.Text("Crashed")
	case sim.Done:
		imgui.Text("Done")
	case sim.Paused:
		if w.toolbarButton("SimPlay", "play-black") {
			w.sim.SetRunning(true)
		}
	case sim.Running:
		if w.toolbarButton("SimPause", "pause-black") {
			w.sim.SetRunning(false)
		}
	}
	imgui.SameLine()
	imgui.BeginDisabledV(w.sim.State != sim.Paused)
	if w.toolbarButton("SimStep1", "step-forward-black") {
		w.sim.Step(10000000)
	}
	if imgui.IsItemHovered() {
		imgui.SetTooltip("10ms")
	}
	imgui.SameLine()
	if w.toolbarButton("SimStep2", "step-forward-2-black") {
		w.sim.Step(1000000000)
	}
	if imgui.IsItemHovered() {
		imgui.SetTooltip("1s")
	}
	imgui.EndDisabled()
	imgui.SameLine()
	imgui.BeginDisabledV(w.sim.State != sim.Paused)
	if w.toolbarButton("SimSkip", "skip-forward") {
		currentPlanState := w.sim.PlanState
		w.sim.StepUntil(func() bool {
			return w.sim.PlanState != currentPlanState
		})
	}
	if imgui.IsItemHovered() {
		imgui.SetTooltip("Next plan change")
	}
	imgui.EndDisabled()
	imgui.SameLineV(0, 20)

	imgui.BeginDisabledV(w.sim.Recording)
	if w.toolbarButton("VCDRecord", "video-outline-black") {
		w.sim.SetRecording(true)
	}
	imgui.EndDisabled()
	imgui.SameLine()
	imgui.BeginDisabledV(!w.sim.Recording)
	if w.toolbarButton("VCDStop", "video-off-outline-black") {
		w.sim.SetRecording(false)
	}
	imgui.EndDisabled()
	imgui.SameLineV(0, 20)

	nanos := w.sim.Nanos()
	nanosText := fmt.Sprintf("%d:%02d:%02d.%03d,%03d,%03d",
		(nanos / 3600000000000),
		(nanos/60000000000)%60,
		(nanos/1000000000)%60,
		(nanos/1000000)%1000,
		(nanos/1000)%1000,
		nanos%1000,
	)
	imgui.SetCursorPosY((48 - imgui.CalcTextSize(nanosText).Y) / 2)
	imgui.Text(nanosText)
}

func (w *toolbarWindow) draw() {
	var toolbarFlags imgui.WindowFlags = imgui.WindowFlagsNoDocking |
		imgui.WindowFlagsNoTitleBar |
		imgui.WindowFlagsNoResize |
		imgui.WindowFlagsNoMove |
		imgui.WindowFlagsNoScrollbar |
		imgui.WindowFlagsNoSavedSettings

	vp := imgui.MainViewport()
	imgui.SetNextWindowPos(vp.Pos())
	imgui.SetNextWindowSize(imgui.NewVec2(vp.Size().X, 48))
	imgui.SetNextWindowViewport(vp.ID())
	imgui.PushStyleVarFloat(imgui.StyleVarWindowBorderSize, 0)
	imgui.BeginV("Toolbar", nil, toolbarFlags)
	imgui.PopStyleVar()
	w.drawToolbar()
	imgui.End()
}
