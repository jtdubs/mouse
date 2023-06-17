package ui

import (
	"fmt"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/ui/pkg/mouse"
)

type serialWindow struct {
	autoScroll   bool
	forceScroll  bool
	filter       imgui.TextFilter
	led          bool
	leftSpeed    int32
	rightSpeed   int32
	leftForward  bool
	rightForward bool
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
		imgui.Text(fmt.Sprintf("%v mV", uint16(m.Serial.Report().BatteryVolts)*39))
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

	// Left Speed
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Left Speed: ")
	imgui.TableSetColumnIndex(1)
	leftSpeed := int32(s.leftSpeed)
	imgui.SliderInt("##leftSpeed", &leftSpeed, 0, 255)
	if leftSpeed != s.leftSpeed {
		s.leftSpeed = leftSpeed
		m.Serial.SendCommand(mouse.MouseCommand{
			Type:  mouse.CommandLeftMotorSpeed,
			Value: uint16(leftSpeed),
		})
	}
	imgui.SameLineV(0, 20)
	leftForward := s.leftForward
	imgui.Checkbox("Reverse##Left", &leftForward)
	if leftForward != s.leftForward {
		s.leftForward = leftForward
		m.Serial.SendCommand(mouse.MouseCommand{
			Type: mouse.CommandLeftMotorDir,
			Value: func() uint16 {
				if leftForward {
					return 1
				} else {
					return 0
				}
			}(),
		})
	}

	// Right Speed
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Right Speed: ")
	imgui.TableSetColumnIndex(1)
	rightSpeed := int32(s.rightSpeed)
	imgui.SliderInt("##rightSpeed", &rightSpeed, 0, 255)
	if rightSpeed != s.rightSpeed {
		s.rightSpeed = rightSpeed
		m.Serial.SendCommand(mouse.MouseCommand{
			Type:  mouse.CommandRightMotorSpeed,
			Value: uint16(rightSpeed),
		})
	}
	imgui.SameLineV(0, 20)
	rightForward := s.rightForward
	imgui.Checkbox("Reverse##Right", &rightForward)
	if rightForward != s.rightForward {
		s.rightForward = rightForward
		m.Serial.SendCommand(mouse.MouseCommand{
			Type: mouse.CommandRightMotorDir,
			Value: func() uint16 {
				if rightForward {
					return 1
				} else {
					return 0
				}
			}(),
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
