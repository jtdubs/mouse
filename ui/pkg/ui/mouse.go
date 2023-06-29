package ui

import (
	"fmt"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/ui/pkg/mouse"
)

type mouseWindow struct {
	mouse       *mouse.Mouse
	autoScroll  bool
	forceScroll bool
	filter      imgui.TextFilter
}

func newMouseWindow(m *mouse.Mouse) *mouseWindow {
	return &mouseWindow{
		mouse:      m,
		autoScroll: true,
		filter:     imgui.NewTextFilter(""),
	}
}

func (w *mouseWindow) init() {
}

func (w *mouseWindow) draw() {
	imgui.Begin("Mouse")

	imgui.SeparatorText("Log")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	w.drawLog()
	imgui.EndGroup()

	imgui.Separator()
	imgui.Text(fmt.Sprintf("Status: %s", w.mouse.Status()))

	imgui.End()
}

func (w *mouseWindow) drawLog() {
	// Toolbar
	imgui.Text("Filter: ")
	imgui.SameLine()
	w.filter.DrawV("", 180)
	imgui.SameLineV(0, 20)
	if imgui.Button("Clear") {
		w.mouse.Clear()
	}
	imgui.SameLine()
	copy := imgui.Button("Copy")
	imgui.SameLineV(0, 20)
	wasAutoScroll := w.autoScroll
	imgui.Checkbox("Auto-scroll", &w.autoScroll)
	if !wasAutoScroll && w.autoScroll {
		w.forceScroll = true
	}
	imgui.Separator()

	// Serial output
	footerHeight := imgui.CurrentStyle().ItemSpacing().Y + imgui.FrameHeightWithSpacing()
	if imgui.BeginChildStrV("ScrollingRegion", imgui.Vec2{X: 0, Y: -footerHeight}, false, imgui.WindowFlagsHorizontalScrollbar) {
		if copy {
			imgui.LogToClipboard()
		}

		w.mouse.ForEachMessage(func(line string) {
			if w.filter.PassFilter(line) {
				if w.mouse.Open() {
					imgui.TextUnformatted(line)
				} else {
					imgui.TextDisabled(line)
				}
			}
		})

		if copy {
			imgui.LogFinish()
		}

		if w.forceScroll || (w.autoScroll && imgui.ScrollY() >= imgui.ScrollMaxY()) {
			imgui.SetScrollHereYV(1.0)
		}
		w.forceScroll = false
	}
	imgui.EndChild()
}
