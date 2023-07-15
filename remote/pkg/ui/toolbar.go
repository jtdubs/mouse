package ui

import (
	"github.com/jtdubs/mouse/remote/pkg/mouse"

	imgui "github.com/AllenDang/cimgui-go"
)

type toolbarWindow struct {
	mouse *mouse.Mouse
}

func newToolbarWindow(mouse *mouse.Mouse) *toolbarWindow {
	return &toolbarWindow{
		mouse: mouse,
	}
}

func (w *toolbarWindow) init() {}

func (w *toolbarWindow) toolbarButton(name string, icon string) bool {
	return imgui.ImageButtonV(name, Textures[icon].ID(), imgui.NewVec2(24, 24), imgui.NewVec2(0, 0), imgui.NewVec2(1, 1), imgui.NewVec4(0, 0, 0, 0), imgui.NewVec4(1, 1, 1, 1))
}

func (w *toolbarWindow) drawToolbar() {
	imgui.BeginDisabledV(!w.mouse.Open())
	if w.toolbarButton("Reset", "restart") {
		w.mouse.SendCommand(mouse.NewResetCommand())
	}
	if imgui.IsItemHovered() {
		imgui.SetTooltip("Reset Mouse")
	}
	imgui.SameLine()
	if w.toolbarButton("Explore", "navigation") {
		w.mouse.SendCommand(mouse.NewExploreCommand())
	}
	if imgui.IsItemHovered() {
		imgui.SetTooltip("Explore")
	}
	imgui.SameLine()
	if w.toolbarButton("Solve", "flag-checkered") {
		w.mouse.SendCommand(mouse.NewSolveCommand())
	}
	if imgui.IsItemHovered() {
		imgui.SetTooltip("Solve")
	}
	imgui.EndDisabled()
	imgui.SameLineV(0, 20)
	if w.mouse.Updating {
		if w.toolbarButton("Freeze UI", "pause-black") {
			w.mouse.SetUpdating(false)
		}
		if imgui.IsItemHovered() {
			imgui.SetTooltip("Freeze UI")
		}
	} else {
		if w.toolbarButton("Thaw UI", "play-black") {
			w.mouse.SetUpdating(true)
		}
		if imgui.IsItemHovered() {
			imgui.SetTooltip("Thaw UI")
		}
	}
	imgui.SameLineV(0, 20)
	imgui.BeginDisabledV(w.mouse.Recording)
	if w.toolbarButton("VCDRecord", "video-outline-black") {
		w.mouse.SetRecording(true)
	}
	imgui.EndDisabled()
	imgui.SameLine()
	imgui.BeginDisabledV(!w.mouse.Recording)
	if w.toolbarButton("VCDStop", "video-off-outline-black") {
		w.mouse.SetRecording(false)
	}
	imgui.EndDisabled()
	imgui.SameLineV(0, 20)
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
	if imgui.BeginV("Toolbar", nil, toolbarFlags) {
		imgui.PopStyleVar()
		w.drawToolbar()
		imgui.End()
	} else {
		imgui.PopStyleVar()
	}
}
