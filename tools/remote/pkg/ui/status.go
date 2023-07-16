package ui

import (
	"fmt"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/tools/remote/pkg/mouse"
)

type statusWindow struct {
	mouse *mouse.Mouse
}

func newStatusWindow(m *mouse.Mouse) *statusWindow {
	return &statusWindow{
		mouse: m,
	}
}

func (w *statusWindow) init() {
}

func (w *statusWindow) drawStatus() {
	if !imgui.Begin("Status") {
		return
	}
	imgui.Text(fmt.Sprintf("Status: %s", w.mouse.Status()))
	imgui.End()
}

func (w *statusWindow) draw() {
	var toolbarFlags imgui.WindowFlags = imgui.WindowFlagsNoDocking |
		imgui.WindowFlagsNoTitleBar |
		imgui.WindowFlagsNoResize |
		imgui.WindowFlagsNoMove |
		imgui.WindowFlagsNoScrollbar |
		imgui.WindowFlagsNoSavedSettings

	vp := imgui.MainViewport()
	imgui.SetNextWindowPos(vp.Pos().Add(imgui.NewVec2(0, vp.Size().Y-40)))
	imgui.SetNextWindowSize(imgui.NewVec2(vp.Size().X, 40))
	imgui.SetNextWindowViewport(vp.ID())
	imgui.PushStyleVarFloat(imgui.StyleVarWindowBorderSize, 0)
	imgui.BeginV("Status", nil, toolbarFlags)
	imgui.PopStyleVar()
	w.drawStatus()
	imgui.End()
}
