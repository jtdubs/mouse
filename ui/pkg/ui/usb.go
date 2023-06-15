package ui

import (
	"fmt"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/ui/pkg/mouse"
)

type usbWindow struct {
	autoScroll  bool
	forceScroll bool
	filter      imgui.TextFilter
}

func newUSBWindow() *usbWindow {
	return &usbWindow{
		autoScroll: true,
		filter:     imgui.NewTextFilter(""),
	}
}

func (s *usbWindow) draw(mouse *mouse.Mouse) {
	if mouse.USB == nil {
		return
	}

	imgui.Begin(fmt.Sprintf("Port - %s", mouse.USB.PortName()))

	// Battery Voltage
	imgui.Text("Battery Voltage: ")
	imgui.SameLine()
	if mouse.USB.Open() {
		imgui.Text(fmt.Sprintf("%v mV", mouse.USB.Report().BatteryVolts))
	} else {
		imgui.Text("Disconnected")
	}
	imgui.Separator()

	// Function Select
	imgui.Text("Function Select: ")
	imgui.SameLine()
	fselButton, fsel := mouse.USB.Report().DecodeFunctionSelect()
	imgui.Text(fmt.Sprintf("%v", fsel))
	if fselButton {
		imgui.SameLine()
		imgui.Text("(pressed)")
	}
	imgui.Separator()

	// Left Encoder
	imgui.Text("Left Encoder: ")
	imgui.SameLine()
	imgui.Text(fmt.Sprintf("%v", mouse.USB.Report().LeftEncoder))
	imgui.Separator()

	// Right Encoder
	imgui.Text("Right Encoder: ")
	imgui.SameLine()
	imgui.Text(fmt.Sprintf("%v", mouse.USB.Report().RightEncoder))
	imgui.Separator()

	// 255 = Button is pressed
	// v := mouse.USB.Report().ButtonState

	// LED
	led := mouse.USB.Command().LED != 0
	imgui.Text("LED: ")
	imgui.SameLine()
	imgui.Checkbox(" ", &led)
	if led != (mouse.USB.Command().LED != 0) {
		if led {
			mouse.USB.Command().LED = 1
		} else {
			mouse.USB.Command().LED = 0
		}
		mouse.USB.SendCommand()
	}
	imgui.Separator()

	// Toolbar
	imgui.Text("Filter: ")
	imgui.SameLine()
	s.filter.DrawV("", 180)
	imgui.SameLineV(0, 20)
	if imgui.Button("Clear") {
		mouse.USB.Clear()
	}
	imgui.SameLine()
	copy := imgui.Button("Copy")
	imgui.SameLineV(0, 20)
	wasAutoScroll := s.autoScroll
	imgui.Checkbox("Auto-scroll", &s.autoScroll)
	if !wasAutoScroll && s.autoScroll {
		s.forceScroll = true
	}
	imgui.Separator()

	// USB output
	footerHeight := imgui.CurrentStyle().ItemSpacing().Y + imgui.FrameHeightWithSpacing()
	if imgui.BeginChildStrV("ScrollingRegion", imgui.Vec2{X: 0, Y: -footerHeight}, false, imgui.WindowFlagsHorizontalScrollbar) {
		if copy {
			imgui.LogToClipboard()
		}

		mouse.USB.ForEachMessage(func(line string) {
			if s.filter.PassFilter(line) {
				if mouse.USB.Open() {
					imgui.TextUnformatted(line)
				} else {
					imgui.TextDisabled(line)
				}
			}
		})

		if copy {
			imgui.LogFinish()
		}

		if s.forceScroll || (s.autoScroll && imgui.ScrollY() >= imgui.ScrollMaxY()) {
			imgui.SetScrollHereYV(1.0)
		}
		s.forceScroll = false
	}
	imgui.EndChild()
	imgui.Separator()

	// Status
	imgui.Text(fmt.Sprintf("Status: %s", mouse.USB.Status()))

	imgui.End()
}
