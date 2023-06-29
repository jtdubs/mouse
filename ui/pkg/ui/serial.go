package ui

import (
	"fmt"
	"math"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/ui/pkg/mouse"
)

type serialWindow struct {
	mouse           *mouse.Mouse
	autoScroll      bool
	forceScroll     bool
	filter          imgui.TextFilter
	linkedSpeeds    bool
	linkedPositions bool
}

func newSerialWindow(m *mouse.Mouse) *serialWindow {
	return &serialWindow{
		mouse:      m,
		autoScroll: true,
		filter:     imgui.NewTextFilter(""),
	}
}

func (s *serialWindow) init() {
}

func (s *serialWindow) draw() {
	if s.mouse.Serial == nil {
		return
	}

	imgui.Begin("Serial")

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

	// LEDs
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

	// Speed
	{
		imgui.Checkbox("Linked Speeds", &s.linkedSpeeds)
		if s.linkedSpeeds {
			leftSetpoint := r.SpeedSetpointLeft
			rightSetpoint := r.SpeedSetpointRight

			changed := s.drawSpeedControl("Speed", &leftSetpoint)

			if changed || leftSetpoint != rightSetpoint {
				s.mouse.Serial.SendCommand(mouse.NewSpeedCommand(leftSetpoint, leftSetpoint))
			}
		} else {
			leftSetpoint := r.SpeedSetpointLeft
			rightSetpoint := r.SpeedSetpointRight

			leftChanged := s.drawSpeedControl("Left Speed", &leftSetpoint)
			rightChanged := s.drawSpeedControl("Right Speed", &rightSetpoint)

			if leftChanged || rightChanged {
				s.mouse.Serial.SendCommand(mouse.NewSpeedCommand(leftSetpoint, rightSetpoint))
			}
		}
	}

	// Position
	{
		imgui.Checkbox("Linked Positions", &s.linkedPositions)
		if s.linkedPositions {
			leftSetpoint := r.PositionSetpointLeft
			rightSetpoint := r.PositionSetpointRight

			changed := s.drawPositionControl("Position", &leftSetpoint)

			if changed || leftSetpoint != rightSetpoint {
				s.mouse.Serial.SendCommand(mouse.NewPositionCommand(leftSetpoint, leftSetpoint))
			}
		} else {
			leftSetpoint := r.PositionSetpointLeft
			rightSetpoint := r.PositionSetpointRight

			leftChanged := s.drawPositionControl("Left Position", &leftSetpoint)
			rightChanged := s.drawPositionControl("Right Position", &rightSetpoint)

			if leftChanged || rightChanged {
				s.mouse.Serial.SendCommand(mouse.NewPositionCommand(leftSetpoint, rightSetpoint))
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

func (s *serialWindow) drawSpeedControl(name string, rpms *float32) (changed bool) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	changed = imgui.SliderFloat(name, rpms, -200, 200)
	if math.Abs(float64(*rpms)) < 20 {
		*rpms = 0
	}
	return
}

func (s *serialWindow) drawPositionControl(name string, mms *float32) (changed bool) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	return imgui.SliderFloat(name, mms, 0, 16.0*180.0)
}
