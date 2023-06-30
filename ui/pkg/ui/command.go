package ui

import (
	"math"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/ui/pkg/mouse"
)

type commandWindow struct {
	mouse           *mouse.Mouse
	linkedPowers    bool
	linkedSpeeds    bool
	linkedPositions bool
}

func newCommandWindow(m *mouse.Mouse) *commandWindow {
	return &commandWindow{
		mouse:           m,
		linkedPowers:    true,
		linkedSpeeds:    true,
		linkedPositions: true,
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
		w.tableRow("Function:")
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

	// Power
	{
		left, right := int32(r.MotorPowerLeft), int32(r.MotorPowerRight)
		values := [2]int32{left, right}

		w.tableRow("Power:")
		w.drawIconToggleButton("##LinkPowers", "link", "link-off", &w.linkedPowers)
		imgui.SameLineV(0, 20)
		if w.linkedPowers {
			if imgui.InputIntV("rpm", &left, 1, 10, imgui.InputTextFlagsEnterReturnsTrue) {
				w.mouse.SendCommand(mouse.NewPowerCommand(int16(left), int16(left)))
			}
		} else {
			if imgui.InputInt2V("rpm", &values, imgui.InputTextFlagsEnterReturnsTrue) {
				w.mouse.SendCommand(mouse.NewPowerCommand(int16(values[0]), int16(values[1])))
			}
		}
	}

	// Speed
	{
		left, right := r.SpeedSetpointLeft, r.SpeedSetpointRight
		pid := [3]float32{r.SpeedKp, r.SpeedKi, r.SpeedKd}
		values := [2]float32{left, right}

		w.tableRow("Speed:")
		w.drawIconToggleButton("##LinkSpeeds", "link", "link-off", &w.linkedSpeeds)
		imgui.SameLineV(0, 20)
		if w.linkedSpeeds {
			if imgui.InputFloatV("rpm", &left, 1, 10, "%.2f", imgui.InputTextFlagsEnterReturnsTrue) {
				if math.Abs(float64(left)) < 20 {
					left = 0
				}
				w.mouse.SendCommand(mouse.NewSpeedCommand(left, left))
			}
		} else {
			if imgui.InputFloat2V("rpm", &values, "%.2f", imgui.InputTextFlagsEnterReturnsTrue) {
				w.mouse.SendCommand(mouse.NewSpeedCommand(values[0], values[1]))
			}
		}
		if imgui.InputFloat3V("PID", &pid, "%.4f", imgui.InputTextFlagsEnterReturnsTrue) {
			w.mouse.SendCommand(mouse.NewSpeedPIDCommand(pid[0], pid[1], pid[2]))
		}
	}

	// Position
	// {
	// 	left, right := r.PositionSetpointLeft, r.PositionSetpointRight
	// 	values := [2]float32{left, right}

	// 	w.tableRow("Position:")
	// 	w.drawIconToggleButton("##LinkPositions", "link", "link-off", &w.linkedPositions)
	// 	imgui.SameLineV(0, 20)
	// 	if w.linkedPositions {
	// 		if imgui.SliderFloat("mm", &left, 0, 16.0*180.0) {
	// 			w.mouse.SendCommand(mouse.NewPositionCommand(left, left))
	// 		}
	// 	} else {
	// 		if imgui.SliderFloat2("mm", &values, 0, 16.0*180.0) {
	// 			w.mouse.SendCommand(mouse.NewSpeedCommand(values[0], values[1]))
	// 		}
	// 	}
	// }

	imgui.EndTable()

	if !w.mouse.Open() {
		imgui.EndDisabled()
	}

	imgui.End()
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
