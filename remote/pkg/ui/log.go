package ui

import (
	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/remote/pkg/mouse"
)

type logWindow struct {
	mouse       *mouse.Mouse
	autoScroll  bool
	forceScroll bool
	filter      imgui.TextFilter
}

func newLogWindow(m *mouse.Mouse) *logWindow {
	return &logWindow{
		mouse:      m,
		autoScroll: true,
		filter:     imgui.NewTextFilter(""),
	}
}

func (w *logWindow) init() {
}

func (w *logWindow) draw() {
	imgui.Begin("Log")

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

	imgui.End()
}
