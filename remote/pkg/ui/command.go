package ui

import (
	"math"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/remote/pkg/mouse"
)

type commandWindow struct {
	mouse                 *mouse.Mouse
	linkedPowers          bool
	linkedSpeeds          bool
	linkedPositions       bool
	powerLeft, powerRight int32
	speedLeft, speedRight float32
	linearDistance        float32
	linearStop            bool
	speedKp, speedKi      float32
}

func newCommandWindow(m *mouse.Mouse) *commandWindow {
	return &commandWindow{
		mouse:           m,
		linkedPowers:    true,
		linkedSpeeds:    true,
		linkedPositions: true,
		speedKp:         0.3,
		speedKi:         8.0 * 0.005,
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

	imgui.BeginTable("##Controls", 3)
	imgui.TableSetupColumnV("##ControlsLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##ControlsControl", imgui.TableColumnFlagsWidthStretch, 0, 0)
	imgui.TableSetupColumnV("##ControlsButton", imgui.TableColumnFlagsWidthFixed, 48, 0)

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

	// Speed PID
	{
		w.tableRow("Speed PID:")
		pid := [2]float32{w.speedKp, w.speedKi}
		if imgui.InputFloat2V("##SpeedPID", &pid, "%.4f", 0) {
			w.speedKp, w.speedKi = pid[0], pid[1]
		}
		imgui.TableSetColumnIndex(2)
		if w.toolbarButton("##SpeedPIDSend", "play-black") {
			w.mouse.SendCommand(mouse.NewSpeedPIDCommand(w.speedKp, w.speedKi))
		}
	}

	// Power
	{
		w.tableRow("Power:")
		w.drawIconToggleButton("##LinkPowers", "link", "link-off", &w.linkedPowers)
		imgui.SameLineV(0, 20)
		if w.linkedPowers {
			if imgui.InputInt("##power", &w.powerLeft) {
				w.powerRight = w.powerLeft
			}
		} else {
			values := [2]int32{w.powerLeft, w.powerRight}
			if imgui.InputInt2("##power", &values) {
				w.powerLeft, w.powerRight = values[0], values[1]
			}
		}
		imgui.TableSetColumnIndex(2)
		if w.toolbarButton("##PowerPlan", "plus-thick") {
			w.mouse.SendCommand(mouse.NewPowerPlanCommand(int16(w.powerLeft), int16(w.powerRight)))
		}
	}

	// Speed
	{

		w.tableRow("Speed (RPM):")
		w.drawIconToggleButton("##LinkSpeeds", "link", "link-off", &w.linkedSpeeds)
		imgui.SameLineV(0, 20)
		if w.linkedSpeeds {
			if imgui.InputFloatV("##rpm", &w.speedLeft, 1, 10, "%.2f", 0) {
				if math.Abs(float64(w.speedLeft)) < 20 {
					w.speedLeft = 0
				}
				w.speedRight = w.speedLeft
			}
		} else {
			values := [2]float32{w.speedLeft, w.speedRight}
			if imgui.InputFloat2V("##rpm", &values, "%.2f", 0) {
				w.speedLeft, w.speedRight = values[0], values[1]
			}
		}
		imgui.TableSetColumnIndex(2)
		if w.toolbarButton("##SpeedPlan", "plus-thick") {
			w.mouse.SendCommand(mouse.NewSpeedPlanCommand(w.speedLeft, w.speedRight))
		}
	}

	// Linear
	{
		w.tableRow("Linear:")
		imgui.InputFloat("mm", &w.linearDistance)
		imgui.SameLine()
		imgui.Checkbox("stop", &w.linearStop)
		imgui.TableSetColumnIndex(2)
		if w.toolbarButton("##LinearPlan", "plus-thick") {
			w.mouse.SendCommand(mouse.NewLinearPlanCommand(w.linearDistance, w.linearStop))
		}
	}

	// Execute
	{
		w.tableRow("")
		if w.toolbarButton("##Execute", "play-black") {
			w.mouse.SendCommand(mouse.NewExecutePlanCommand())
		}
	}

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

func (w *commandWindow) toolbarButton(name string, icon string) bool {
	return imgui.ImageButtonV(name, Textures[icon].ID(), imgui.NewVec2(24, 24), imgui.NewVec2(0, 0), imgui.NewVec2(1, 1), imgui.NewVec4(0, 0, 0, 0), imgui.NewVec4(1, 1, 1, 1))
}
