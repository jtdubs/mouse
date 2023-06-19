package ui

import (
	"fmt"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/ui/pkg/mouse"
)

type serialWindow struct {
	mouse        *mouse.Mouse
	autoScroll   bool
	forceScroll  bool
	filter       imgui.TextFilter
	onboardLED   bool
	leftLED      bool
	rightLED     bool
	irLEDs       bool
	leftSpeed    int32
	rightSpeed   int32
	leftForward  bool
	rightForward bool
}

func newSerialWindow(m *mouse.Mouse) *serialWindow {
	return &serialWindow{
		mouse:      m,
		autoScroll: true,
		filter:     imgui.NewTextFilter(""),
	}
}

func (s *serialWindow) draw() {
	if s.mouse.Serial == nil {
		return
	}

	imgui.Begin(fmt.Sprintf("Port - %s", s.mouse.Serial.PortName()))

	imgui.SeparatorText("Status")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	s.drawStatus()
	imgui.EndGroup()

	imgui.SeparatorText("Controls")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	s.drawControls()
	imgui.EndGroup()

	imgui.SeparatorText("Log")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	s.drawLog()
	imgui.EndGroup()

	imgui.Separator()
	imgui.Text(fmt.Sprintf("Status: %s", s.mouse.Serial.Status()))

	imgui.End()
}

func (s *serialWindow) drawStatus() {
	imgui.BeginTable("##Status", 2)
	imgui.TableSetupColumnV("##StatusLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##StatusControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	// Battery Voltage
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Battery Voltage: ")
	imgui.TableSetColumnIndex(1)
	if s.mouse.Serial.Open() {
		imgui.Text(fmt.Sprintf("%v mV", uint16(s.mouse.Serial.Report().BatteryVolts)*39))
	} else {
		imgui.Text("Disconnected")
	}

	// Function Select
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Function Select: ")
	imgui.TableSetColumnIndex(1)
	if s.mouse.Serial.Open() {
		fselButton, fsel := s.mouse.Serial.Report().DecodeFunctionSelect()
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
	if s.mouse.Serial.Open() {
		imgui.Text(fmt.Sprintf("%v", s.mouse.Serial.Report().LeftEncoder))
	} else {
		imgui.Text("Disconnected")
	}

	// Right Encoder
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text("Right Encoder: ")
	imgui.TableSetColumnIndex(1)
	if s.mouse.Serial.Open() {
		imgui.Text(fmt.Sprintf("%v", s.mouse.Serial.Report().RightEncoder))
	} else {
		imgui.Text("Disconnected")
	}

	imgui.EndTable()
}

func (s *serialWindow) drawControls() {
	if !s.mouse.Serial.Open() {
		imgui.BeginDisabled()
	}

	imgui.BeginTable("##Controls", 2)
	imgui.TableSetupColumnV("##ControlsLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##ControlsControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	s.drawLEDControl("Onboard LED", mouse.CommandOnboardLED, &s.onboardLED)
	s.drawLEDControl("Left LED", mouse.CommandLeftLED, &s.leftLED)
	s.drawLEDControl("Right LED", mouse.CommandRightLED, &s.rightLED)
	s.drawLEDControl("IR LEDs", mouse.CommandIRLEDs, &s.irLEDs)
	s.drawMotorControl("Left Motor", mouse.CommandLeftMotorSpeed, &s.leftSpeed, mouse.CommandLeftMotorDir, &s.leftForward)
	s.drawMotorControl("Right Motor", mouse.CommandRightMotorSpeed, &s.rightSpeed, mouse.CommandRightMotorDir, &s.rightForward)

	imgui.EndTable()

	if !s.mouse.Serial.Open() {
		imgui.EndDisabled()
	}
}

func (s *serialWindow) drawLog() {
	// Toolbar
	imgui.Text("Filter: ")
	imgui.SameLine()
	s.filter.DrawV("", 180)
	imgui.SameLineV(0, 20)
	if imgui.Button("Clear") {
		s.mouse.Serial.Clear()
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

		s.mouse.Serial.ForEachMessage(func(line string) {
			if s.filter.PassFilter(line) {
				if s.mouse.Serial.Open() {
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

func (s *serialWindow) drawLEDControl(name string, commandType mouse.CommandType, value *bool) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	temp := *value
	imgui.Checkbox(fmt.Sprintf("##%v", name), &temp)
	if temp != *value {
		*value = temp
		s.mouse.Serial.SendCommand(mouse.MouseCommand{
			Type: commandType,
			Value: func() uint16 {
				if temp {
					return 1
				} else {
					return 0
				}
			}(),
		})
	}
}

func (s *serialWindow) drawMotorControl(name string, speedCommand mouse.CommandType, speed *int32, dirCommand mouse.CommandType, forward *bool) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	tempSpeed := int32(*speed)
	imgui.SliderInt(fmt.Sprintf("##%vSpeed", name), &tempSpeed, 0, 255)
	if tempSpeed != *speed {
		*speed = tempSpeed
		s.mouse.Serial.SendCommand(mouse.MouseCommand{
			Type:  speedCommand,
			Value: uint16(tempSpeed),
		})
	}
	imgui.SameLineV(0, 20)
	tempForward := *forward
	imgui.Checkbox(fmt.Sprintf("Reverse##%vDirection", name), &tempForward)
	if tempForward != s.leftForward {
		*forward = tempForward
		s.mouse.Serial.SendCommand(mouse.MouseCommand{
			Type: mouse.CommandLeftMotorDir,
			Value: func() uint16 {
				if tempForward {
					return 1
				} else {
					return 0
				}
			}(),
		})
	}
}
