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

func (s *toolbarWindow) init() {}

func (s *toolbarWindow) toolbarButton(name string, icon string) bool {
	return imgui.ImageButtonV(name, Textures[icon].ID(), imgui.NewVec2(24, 24), imgui.NewVec2(0, 0), imgui.NewVec2(1, 1), imgui.NewVec4(0, 0, 0, 0), imgui.NewVec4(1, 1, 1, 1))
}

func (s *toolbarWindow) drawToolbar() {
	switch s.sim.State {
	case sim.Crashed:
		imgui.Text("Crashed")
	case sim.Done:
		imgui.Text("Done")
	case sim.Paused:
		if s.toolbarButton("SimPlay", "play-black") {
			s.sim.SetRunning(true)
		}
	case sim.Running:
		if s.toolbarButton("SimPause", "pause-black") {
			s.sim.SetRunning(false)
		}
	}
	imgui.SameLine()
	imgui.BeginDisabledV(s.sim.State != sim.Paused)
	if s.toolbarButton("SimStep1", "step-forward-black") {
		s.sim.Step(1000000)
	}
	imgui.SameLine()
	if s.toolbarButton("SimStep2", "step-forward-2-black") {
		s.sim.Step(100000000)
	}
	imgui.EndDisabled()
	imgui.SameLineV(0, 20)

	imgui.BeginDisabledV(s.sim.Recording)
	if s.toolbarButton("VCDRecord", "video-outline-black") {
		s.sim.SetRecording(true)
	}
	imgui.EndDisabled()
	imgui.SameLine()
	imgui.BeginDisabledV(!s.sim.Recording)
	if s.toolbarButton("VCDStop", "video-off-outline-black") {
		s.sim.SetRecording(false)
	}
	imgui.EndDisabled()
	imgui.SameLineV(0, 20)

	nanos := s.sim.Nanos()
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

func (s *toolbarWindow) draw() {
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
	s.drawToolbar()
	imgui.End()
}
