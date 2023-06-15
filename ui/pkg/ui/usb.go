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
	led         bool
	leftPWM     int32
	rightPWM    int32
}

func newUSBWindow() *usbWindow {
	return &usbWindow{
		autoScroll: true,
		filter:     imgui.NewTextFilter(""),
	}
}

func (s *usbWindow) draw(m *mouse.Mouse) {
	if m.USB == nil {
		return
	}

	imgui.Begin(fmt.Sprintf("Port - %s", m.USB.PortName()))

	// Battery Voltage
	imgui.Text("Battery Voltage: ")
	imgui.SameLine()
	if m.USB.Open() {
		imgui.Text(fmt.Sprintf("%v mV", m.USB.Report().BatteryVolts))
	} else {
		imgui.Text("Disconnected")
	}
	imgui.Separator()

	// Function Select
	imgui.Text("Function Select: ")
	imgui.SameLine()
	fselButton, fsel := m.USB.Report().DecodeFunctionSelect()
	imgui.Text(fmt.Sprintf("%v", fsel))
	if fselButton {
		imgui.SameLine()
		imgui.Text("(pressed)")
	}
	imgui.Separator()

	// Left Encoder
	imgui.Text("Left Encoder: ")
	imgui.SameLine()
	imgui.Text(fmt.Sprintf("%v", m.USB.Report().LeftEncoder))
	imgui.Separator()

	// Right Encoder
	imgui.Text("Right Encoder: ")
	imgui.SameLine()
	imgui.Text(fmt.Sprintf("%v", m.USB.Report().RightEncoder))
	imgui.Separator()

	if !m.USB.Open() {
		imgui.BeginDisabled()
	}

	// LED
	imgui.Text("LED: ")
	imgui.SameLine()
	led := s.led
	imgui.Checkbox(" ", &led)
	if led != s.led {
		s.led = led
		m.USB.SendCommand(mouse.MouseCommand{
			Type: mouse.CommandLED,
			Value: func() uint16 {
				if led {
					return 1
				} else {
					return 0
				}
			}(),
		})
	}
	imgui.Separator()

	// Left PWM
	imgui.Text("Left PWM: ")
	imgui.SameLine()
	leftPWM := int32(s.leftPWM)
	imgui.SliderInt("##LeftPWM", &leftPWM, 0, 100)
	if leftPWM != s.leftPWM {
		s.leftPWM = leftPWM
		m.USB.SendCommand(mouse.MouseCommand{
			Type:  mouse.CommandPWMLeft,
			Value: uint16(leftPWM),
		})
	}
	imgui.Separator()

	// Right PWM
	imgui.Text("Right PWM: ")
	imgui.SameLine()
	rightPWM := int32(s.rightPWM)
	imgui.SliderInt("##RightPWM", &rightPWM, 0, 100)
	if rightPWM != s.rightPWM {
		s.rightPWM = rightPWM
		m.USB.SendCommand(mouse.MouseCommand{
			Type:  mouse.CommandPWMRight,
			Value: uint16(rightPWM),
		})
	}
	imgui.Separator()

	if !m.USB.Open() {
		imgui.EndDisabled()
	}

	// Toolbar
	imgui.Text("Filter: ")
	imgui.SameLine()
	s.filter.DrawV("", 180)
	imgui.SameLineV(0, 20)
	if imgui.Button("Clear") {
		m.USB.Clear()
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

		m.USB.ForEachMessage(func(line string) {
			if s.filter.PassFilter(line) {
				if m.USB.Open() {
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
	imgui.Text(fmt.Sprintf("Status: %s", m.USB.Status()))

	imgui.End()
}
