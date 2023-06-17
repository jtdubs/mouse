package ui

import (
	"fmt"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/ui/pkg/mouse"
)

type serialWindow struct {
	autoScroll  bool
	forceScroll bool
	filter      imgui.TextFilter
	led         bool
	leftPWM     int32
	rightPWM    int32
}

func newSerialWindow() *serialWindow {
	return &serialWindow{
		autoScroll: true,
		filter:     imgui.NewTextFilter(""),
	}
}

func (s *serialWindow) draw(m *mouse.Mouse) {
	if m.Serial == nil {
		return
	}

	imgui.Begin(fmt.Sprintf("Port - %s", m.Serial.PortName()))

	imgui.SeparatorText("Status")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	s.drawStatus(m)
	imgui.EndGroup()

	imgui.SeparatorText("Controls")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	s.drawControls(m)
	imgui.EndGroup()

	imgui.SeparatorText("Log")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	s.drawLog(m)
	imgui.EndGroup()

	imgui.Separator()
	imgui.Text(fmt.Sprintf("Status: %s", m.Serial.Status()))

	imgui.End()
}

func (s *serialWindow) drawStatus(m *mouse.Mouse) {
	imgui.BeginTable("##Status", 2)
	imgui.TableSetupColumnV("##StatusLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##StatusControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	// Battery Voltage
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Battery Voltage: ")
	imgui.TableSetColumnIndex(1)
	if m.Serial.Open() {
		imgui.Text(fmt.Sprintf("%v mV", m.Serial.Report().BatteryVolts))
	} else {
		imgui.Text("Disconnected")
	}

	// Function Select
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Function Select: ")
	imgui.TableSetColumnIndex(1)
	if m.Serial.Open() {
		fselButton, fsel := m.Serial.Report().DecodeFunctionSelect()
		imgui.Text(fmt.Sprintf("%v", fsel))
		if fselButton {
			imgui.SameLine()
			imgui.Text("(pressed)")
		}
	} else {
		imgui.Text("Disconnected")
	}

	// Left Encoder
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Left Encoder: ")
	imgui.TableSetColumnIndex(1)
	if m.Serial.Open() {
		imgui.Text(fmt.Sprintf("%v", m.Serial.Report().LeftEncoder))
	} else {
		imgui.Text("Disconnected")
	}

	// Right Encoder
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Right Encoder: ")
	imgui.TableSetColumnIndex(1)
	if m.Serial.Open() {
		imgui.Text(fmt.Sprintf("%v", m.Serial.Report().RightEncoder))
	} else {
		imgui.Text("Disconnected")
	}

	imgui.EndTable()
}

func (s *serialWindow) drawControls(m *mouse.Mouse) {
	if !m.Serial.Open() {
		imgui.BeginDisabled()
	}

	imgui.BeginTable("##Controls", 2)
	imgui.TableSetupColumnV("##ControlsLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##ControlsControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	// LED
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("LED: ")
	imgui.TableSetColumnIndex(1)
	led := s.led
	imgui.Checkbox("##LED", &led)
	if led != s.led {
		s.led = led
		m.Serial.SendCommand(mouse.MouseCommand{
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

	// Left PWM
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Left PWM: ")
	imgui.TableSetColumnIndex(1)
	leftPWM := int32(s.leftPWM)
	imgui.SliderInt("##LeftPWM", &leftPWM, 0, 100)
	if leftPWM != s.leftPWM {
		s.leftPWM = leftPWM
		m.Serial.SendCommand(mouse.MouseCommand{
			Type:  mouse.CommandPWMLeft,
			Value: uint16(leftPWM),
		})
	}

	// Right PWM
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Right PWM: ")
	imgui.TableSetColumnIndex(1)
	rightPWM := int32(s.rightPWM)
	imgui.SliderInt("##RightPWM", &rightPWM, 0, 100)
	if rightPWM != s.rightPWM {
		s.rightPWM = rightPWM
		m.Serial.SendCommand(mouse.MouseCommand{
			Type:  mouse.CommandPWMRight,
			Value: uint16(rightPWM),
		})
	}

	imgui.EndTable()

	if !m.Serial.Open() {
		imgui.EndDisabled()
	}
}

func (s *serialWindow) drawLog(m *mouse.Mouse) {
	// Toolbar
	imgui.Text("Filter: ")
	imgui.SameLine()
	s.filter.DrawV("", 180)
	imgui.SameLineV(0, 20)
	if imgui.Button("Clear") {
		m.Serial.Clear()
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

	// Serial output
	footerHeight := imgui.CurrentStyle().ItemSpacing().Y + imgui.FrameHeightWithSpacing()
	if imgui.BeginChildStrV("ScrollingRegion", imgui.Vec2{X: 0, Y: -footerHeight}, false, imgui.WindowFlagsHorizontalScrollbar) {
		if copy {
			imgui.LogToClipboard()
		}

		m.Serial.ForEachMessage(func(line string) {
			if s.filter.PassFilter(line) {
				if m.Serial.Open() {
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
}
