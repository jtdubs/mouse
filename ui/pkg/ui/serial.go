package ui

import (
	"fmt"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/ui/pkg/mouse"
)

type serialWindow struct {
	mouse           *mouse.Mouse
	autoScroll      bool
	forceScroll     bool
	filter          imgui.TextFilter
	linkedThrottles bool
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
	r := s.mouse.Serial.Report()

	imgui.BeginTable("##Status", 2)
	imgui.TableSetupColumnV("##StatusLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##StatusControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	s.drawNumericStatus("Battery Voltage", int(r.BatteryVolts)*39, "mV")

	{
		modes := []string{"Remote", "Wall Sensor", "Error", "Unknown #3", "Unknown #4", "Unknown #5", "Unknown #6", "Unknown #7"}
		imgui.TableNextRow()
		imgui.TableSetColumnIndex(0)
		imgui.Text("Mode:")
		imgui.TableSetColumnIndex(1)
		if s.mouse.Serial.Open() {
			imgui.Text(modes[r.Mode])
		} else {
			imgui.Text("Disconnected")
		}
	}

	s.drawNumericStatus("Left Encoder", int(r.EncoderLeft), "")
	s.drawNumericStatus("Right Encoder", int(r.EncoderRight), "")

	{
		left, center, right := r.DecodeSensors()
		s.drawNumericStatus("Left Sensor", int(left), "")
		s.drawNumericStatus("Center Sensor", int(center), "")
		s.drawNumericStatus("Right Sensor", int(right), "")
	}

	{
		onboard, left, right, ir := r.DecodeLEDs()
		s.drawLEDStatus("Onboard LED", onboard)
		s.drawLEDStatus("Left LED", left)
		s.drawLEDStatus("Right LED", right)
		s.drawLEDStatus("IR LEDs", ir)
	}

	imgui.EndTable()
}

func (s *serialWindow) drawControls() {
	r := s.mouse.Serial.Report()

	if !s.mouse.Serial.Open() {
		imgui.BeginDisabled()
	}

	imgui.BeginTable("##Controls", 2)
	imgui.TableSetupColumnV("##ControlsLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##ControlsControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	// Mode
	{
		mode := int32(r.Mode)
		imgui.TableNextRow()
		imgui.TableSetColumnIndex(0)
		imgui.Text("Function: ")
		imgui.TableSetColumnIndex(1)
		imgui.ComboStr("##FSEL", &mode, "Remote\000Wall Sensor\000Error")
		if mode != int32(r.Mode) {
			s.mouse.Serial.SendCommand(mouse.NewModeCommand(uint8(mode)))
		}
	}

	{
		onboard, left, right, ir := r.DecodeLEDs()
		changed := false
		changed = changed || s.drawLEDControl("Onboard LED", &onboard)
		changed = changed || s.drawLEDControl("Left LED", &left)
		changed = changed || s.drawLEDControl("Right LED", &right)
		changed = changed || s.drawLEDControl("IR LEDs", &ir)
		if changed {
			s.mouse.Serial.SendCommand(mouse.NewLEDCommand(onboard, left, right, ir))
		}
	}

	{
		imgui.Checkbox("Linked Throttles", &s.linkedThrottles)
		if s.linkedThrottles {
			leftForward, rightForward := r.DecodeForward()
			leftSpeed := int32(r.SpeedLeft)
			rightSpeed := int32(r.SpeedRight)

			speed := leftSpeed
			if !leftForward {
				speed = -speed
			}
			changed := s.drawMotorControl("Motors", &speed)
			forward := speed >= 0
			if !forward {
				speed = -speed
			}

			if changed || forward != leftForward || forward != rightForward || speed != leftSpeed || speed != rightSpeed {
				s.mouse.Serial.SendCommand(mouse.NewMotorCommand(uint8(speed), uint8(speed), forward, forward))
			}
		} else {
			leftForward, rightForward := r.DecodeForward()
			leftSpeed := int32(r.SpeedLeft)
			rightSpeed := int32(r.SpeedRight)
			if !leftForward {
				leftSpeed = -leftSpeed
			}
			if !rightForward {
				rightSpeed = -rightSpeed
			}

			changed := false
			changed = s.drawMotorControl("Left Motor", &leftSpeed) || changed
			changed = s.drawMotorControl("Right Motor", &rightSpeed) || changed

			if leftSpeed >= 0 {
				leftForward = true
			} else {
				leftSpeed = -leftSpeed
				leftForward = false
			}

			if rightSpeed >= 0 {
				rightForward = true
			} else {
				rightSpeed = -rightSpeed
				rightForward = false
			}

			if changed {
				s.mouse.Serial.SendCommand(mouse.NewMotorCommand(uint8(leftSpeed), uint8(rightSpeed), leftForward, rightForward))
			}
		}
	}

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

func (s *serialWindow) drawNumericStatus(name string, value int, units string) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	if s.mouse.Serial.Open() {
		imgui.Text(fmt.Sprint(value))
		if units != "" {
			imgui.SameLine()
			imgui.Text(units)
		}
	} else {
		imgui.Text("Disconnected")
	}
}

func (s *serialWindow) drawLEDStatus(name string, value bool) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	if s.mouse.Serial.Open() {
		if value {
			imgui.Text("On")
		} else {
			imgui.Text("Off")
		}
	} else {
		imgui.Text("Disconnected")
	}
}

func (s *serialWindow) drawLEDControl(name string, value *bool) (changed bool) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	oldValue := *value
	imgui.Checkbox(fmt.Sprintf("##%v", name), value)
	return oldValue != *value
}

func (s *serialWindow) drawMotorControl(name string, speed *int32) (changed bool) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	oldSpeed := *speed
	imgui.SliderInt(fmt.Sprintf("##%vSpeed", name), speed, -50, 50)
	return oldSpeed != *speed
}
