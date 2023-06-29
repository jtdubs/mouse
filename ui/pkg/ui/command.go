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
		changed := false
		onboard, left, right, ir := r.DecodeLEDs()
		w.tableRow("Status LEDs:")
		changed = w.drawIconToggleButton("##LeftLED", "led-off-white", "led-on-lightblue", &left) || changed
		imgui.SameLineV(0, 20)
		changed = w.drawIconToggleButton("##OnboardLED", "led-off-white", "led-on-orange", &onboard) || changed
		imgui.SameLineV(0, 20)
		changed = w.drawIconToggleButton("##RightLED", "led-off-white", "led-on-lightblue", &right) || changed
		w.tableRow("IR LEDs:")
		changed = w.drawIconToggleButton("##IR1", "led-off-white", "led-on-red", &ir) || changed
		imgui.SameLineV(0, 20)
		changed = w.drawIconToggleButton("##IR2", "led-off-white", "led-on-red", &ir) || changed
		imgui.SameLineV(0, 20)
		changed = w.drawIconToggleButton("##IR3", "led-off-white", "led-on-red", &ir) || changed
		if changed {
			w.mouse.SendCommand(mouse.NewLEDCommand(onboard, left, right, ir))
		}
	}

	// Speed
	{
		imgui.Checkbox("Linked Speeds", &w.linkedSpeeds)
		if w.linkedSpeeds {
			leftSetpoint := r.SpeedSetpointLeft
			changed := w.drawSpeedControl("Speed", &leftSetpoint)
			if changed {
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
			changed := w.drawPositionControl("Position", &leftSetpoint)
			if changed {
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

func (w *commandWindow) drawIconButton(label, name string) bool {
	return imgui.ImageButtonV(label, Textures[name].ID(), imgui.NewVec2(24, 24), imgui.NewVec2(0, 0), imgui.NewVec2(1, 1), imgui.NewVec4(0, 0, 0, 0), imgui.NewVec4(1, 1, 1, 1))
}

func (w *commandWindow) drawIconToggleButton(label, off, on string, value *bool) bool {
	var texture imgui.TextureID
	if *value {
		texture = Textures[on].ID()
	} else {
		texture = Textures[off].ID()
	}

	if imgui.ImageButtonV(label, texture, imgui.NewVec2(24, 24), imgui.NewVec2(0, 0), imgui.NewVec2(1, 1), imgui.NewVec4(0, 0, 0, 0), imgui.NewVec4(1, 1, 1, 1)) {
		*value = !*value
		return true
	}

	return false
}

func (w *commandWindow) tableRow(label string) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(label)
	imgui.TableSetColumnIndex(1)
}
