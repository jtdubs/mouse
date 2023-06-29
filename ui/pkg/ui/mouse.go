package ui

import (
	"fmt"
	"math"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/ui/pkg/mouse"
)

type mouseWindow struct {
	mouse           *mouse.Mouse
	autoScroll      bool
	forceScroll     bool
	filter          imgui.TextFilter
	linkedSpeeds    bool
	linkedPositions bool
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
	imgui.Begin("Serial")

	imgui.SeparatorText("Status")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	w.drawStatus()
	imgui.EndGroup()

	imgui.SeparatorText("Controls")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	w.drawControls()
	imgui.EndGroup()

	imgui.SeparatorText("Log")
	imgui.Text("")
	imgui.SameLineV(0, 20)
	imgui.BeginGroup()
	w.drawLog()
	imgui.EndGroup()

	imgui.Separator()
	imgui.Text(fmt.Sprintf("Status: %w", w.mouse.Status()))

	imgui.End()
}

func (w *mouseWindow) drawStatus() {
	r := w.mouse.Report()

	imgui.BeginTable("##Status", 2)
	imgui.TableSetupColumnV("##StatusLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##StatusControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	w.drawNumericStatus("Battery Voltage", int(r.BatteryVolts)*39, "mV")

	{
		modes := []string{"Remote", "Wall Sensor", "Error", "Unknown #3", "Unknown #4", "Unknown #5", "Unknown #6", "Unknown #7"}
		imgui.TableNextRow()
		imgui.TableSetColumnIndex(0)
		imgui.Text("Mode:")
		imgui.TableSetColumnIndex(1)
		if w.mouse.Open() {
			imgui.Text(modes[r.Mode])
		} else {
			imgui.Text("Disconnected")
		}
	}

	w.drawNumericStatus("Left Encoder", int(r.EncoderLeft), "")
	w.drawNumericStatus("Right Encoder", int(r.EncoderRight), "")

	{
		left, center, right := r.DecodeSensors()
		w.drawNumericStatus("Left Sensor", int(left), "")
		w.drawNumericStatus("Center Sensor", int(center), "")
		w.drawNumericStatus("Right Sensor", int(right), "")
	}

	{
		onboard, left, right, ir := r.DecodeLEDs()
		w.drawLEDStatus("Onboard LED", onboard)
		w.drawLEDStatus("Left LED", left)
		w.drawLEDStatus("Right LED", right)
		w.drawLEDStatus("IR LEDs", ir)
	}

	imgui.EndTable()
}

func (w *mouseWindow) drawControls() {
	r := w.mouse.Report()

	if !w.mouse.Open() {
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
			w.mouse.SendCommand(mouse.NewModeCommand(uint8(mode)))
		}
	}

	// LEDs
	{
		onboard, left, right, ir := r.DecodeLEDs()
		changed := false
		changed = changed || w.drawLEDControl("Onboard LED", &onboard)
		changed = changed || w.drawLEDControl("Left LED", &left)
		changed = changed || w.drawLEDControl("Right LED", &right)
		changed = changed || w.drawLEDControl("IR LEDs", &ir)
		if changed {
			w.mouse.SendCommand(mouse.NewLEDCommand(onboard, left, right, ir))
		}
	}

	// Speed
	{
		imgui.Checkbox("Linked Speeds", &w.linkedSpeeds)
		if w.linkedSpeeds {
			leftSetpoint := r.SpeedSetpointLeft
			rightSetpoint := r.SpeedSetpointRight

			changed := w.drawSpeedControl("Speed", &leftSetpoint)

			if changed || leftSetpoint != rightSetpoint {
				w.mouse.SendCommand(mouse.NewSpeedCommand(leftSetpoint, leftSetpoint))
			}
		} else {
			leftSetpoint := r.SpeedSetpointLeft
			rightSetpoint := r.SpeedSetpointRight

			leftChanged := w.drawSpeedControl("Left Speed", &leftSetpoint)
			rightChanged := w.drawSpeedControl("Right Speed", &rightSetpoint)

			if leftChanged || rightChanged {
				w.mouse.SendCommand(mouse.NewSpeedCommand(leftSetpoint, rightSetpoint))
			}
		}
	}

	// Position
	{
		imgui.Checkbox("Linked Positions", &w.linkedPositions)
		if w.linkedPositions {
			leftSetpoint := r.PositionSetpointLeft
			rightSetpoint := r.PositionSetpointRight

			changed := w.drawPositionControl("Position", &leftSetpoint)

			if changed || leftSetpoint != rightSetpoint {
				w.mouse.SendCommand(mouse.NewPositionCommand(leftSetpoint, leftSetpoint))
			}
		} else {
			leftSetpoint := r.PositionSetpointLeft
			rightSetpoint := r.PositionSetpointRight

			leftChanged := w.drawPositionControl("Left Position", &leftSetpoint)
			rightChanged := w.drawPositionControl("Right Position", &rightSetpoint)

			if leftChanged || rightChanged {
				w.mouse.SendCommand(mouse.NewPositionCommand(leftSetpoint, rightSetpoint))
			}
		}
	}

	imgui.EndTable()

	if !w.mouse.Open() {
		imgui.EndDisabled()
	}
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

func (w *mouseWindow) drawNumericStatus(name string, value int, units string) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	if w.mouse.Open() {
		imgui.Text(fmt.Sprint(value))
		if units != "" {
			imgui.SameLine()
			imgui.Text(units)
		}
	} else {
		imgui.Text("Disconnected")
	}
}

func (w *mouseWindow) drawLEDStatus(name string, value bool) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	if w.mouse.Open() {
		if value {
			imgui.Text("On")
		} else {
			imgui.Text("Off")
		}
	} else {
		imgui.Text("Disconnected")
	}
}

func (w *mouseWindow) drawLEDControl(name string, value *bool) (changed bool) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	oldValue := *value
	imgui.Checkbox(fmt.Sprintf("##%v", name), value)
	return oldValue != *value
}

func (w *mouseWindow) drawSpeedControl(name string, rpms *float32) (changed bool) {
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

func (w *mouseWindow) drawPositionControl(name string, mms *float32) (changed bool) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	return imgui.SliderFloat(name, mms, 0, 16.0*180.0)
}
