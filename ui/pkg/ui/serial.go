package ui

import (
	"fmt"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/ui/pkg/mouse"
)

type serialWindow struct {
	mouse       *mouse.Mouse
	autoScroll  bool
	forceScroll bool
	filter      imgui.TextFilter
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
			s.mouse.Serial.SendCommand(mouse.MouseCommand{
				Type:  mouse.CommandSetMode,
				Value: uint16(mode),
			})
		}
	}

	{
		onboard, left, right, ir := r.DecodeLEDs()
		s.drawLEDControl("Onboard LED", mouse.CommandOnboardLED, onboard)
		s.drawLEDControl("Left LED", mouse.CommandLeftLED, left)
		s.drawLEDControl("Right LED", mouse.CommandRightLED, right)
		s.drawLEDControl("IR LEDs", mouse.CommandIRLEDs, ir)
	}

	{
		left, right := r.DecodeForward()
		s.drawMotorControl("Left Motor", mouse.CommandLeftMotorSpeed, mouse.CommandLeftMotorForward, int32(r.SpeedLeft), left)
		s.drawMotorControl("Right Motor", mouse.CommandRightMotorSpeed, mouse.CommandRightMotorForward, int32(r.SpeedRight), right)
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

func (s *serialWindow) drawLEDControl(name string, commandType mouse.CommandType, value bool) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	temp := value
	imgui.Checkbox(fmt.Sprintf("##%v", name), &temp)
	if temp != value {
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

func (s *serialWindow) drawMotorControl(name string, speedCommand mouse.CommandType, dirCommand mouse.CommandType, speed int32, forward bool) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	tempSpeed := speed
	imgui.SliderInt(fmt.Sprintf("##%vSpeed", name), &tempSpeed, 0, 255)
	if tempSpeed != speed {
		s.mouse.Serial.SendCommand(mouse.MouseCommand{
			Type:  speedCommand,
			Value: uint16(tempSpeed),
		})
	}
	imgui.SameLineV(0, 20)
	tempForward := forward
	imgui.Checkbox(fmt.Sprintf("Forward##%vForward", name), &tempForward)
	if tempForward != forward {
		s.mouse.Serial.SendCommand(mouse.MouseCommand{
			Type: dirCommand,
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
