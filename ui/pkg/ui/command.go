package ui

import (
	"fmt"
	"math"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/ui/pkg/mouse"
)

type commandWindow struct {
	mouse           *mouse.Mouse
	linkedSpeeds    bool
	linkedPositions bool
}

func newCommandWindow(m *mouse.Mouse) *commandWindow {
	return &commandWindow{
		mouse: m,
	}
}

func (w *commandWindow) init() {
}

func (w *commandWindow) draw() {
	imgui.Begin("Command")

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

	imgui.End()
}

func (w *commandWindow) drawLEDControl(name string, value *bool) (changed bool) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	oldValue := *value
	imgui.Checkbox(fmt.Sprintf("##%v", name), value)
	return oldValue != *value
}

func (w *commandWindow) drawSpeedControl(name string, rpms *float32) (changed bool) {
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

func (w *commandWindow) drawPositionControl(name string, mms *float32) (changed bool) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(fmt.Sprintf("%v:", name))
	imgui.TableSetColumnIndex(1)
	return imgui.SliderFloat(name, mms, 0, 16.0*180.0)
}
