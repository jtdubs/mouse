package ui

import (
	"math"

	imgui "github.com/AllenDang/cimgui-go"
	"github.com/jtdubs/mouse/remote/pkg/mouse"
)

type commandWindow struct {
	mouse                                 *mouse.Mouse
	linkedPowers                          bool
	linkedSpeeds                          bool
	linkedPositions                       bool
	powerLeft, powerRight                 int32
	speedLeft, speedRight                 float32
	linearDistance                        float32
	linearStop                            bool
	rotationalDTheta                      float32
	speedKp, speedKi, speedKd, speedAlpha float32
	wallKp, wallKi, wallKd, wallAlpha     float32
	angleKp, angleKi, angleKd, angleAlpha float32
	ledLeft, ledBuiltin, ledRight, ledIR  bool
}

func newCommandWindow(m *mouse.Mouse) *commandWindow {
	return &commandWindow{
		mouse:           m,
		linkedPowers:    true,
		linkedSpeeds:    true,
		linkedPositions: true,
		speedKp:         0.1,
		speedKi:         0.2 * 0.005,
		speedKd:         0.0,
		speedAlpha:      0.5,
		wallKp:          0.1,
		wallKi:          0.0,
		wallKd:          0.0,
		wallAlpha:       0.5,
		angleKp:         0.1,
		angleKi:         0.0,
		angleKd:         0.0,
		angleAlpha:      0.5,
	}
}

func (w *commandWindow) init() {
}

func (w *commandWindow) draw() {
	if !imgui.Begin("Command") {
		return
	}

	if !w.mouse.Open() {
		imgui.BeginDisabled()
	}

	imgui.BeginTable("##Controls", 3)
	imgui.TableSetupColumnV("##ControlsLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##ControlsControl", imgui.TableColumnFlagsWidthStretch, 0, 0)
	imgui.TableSetupColumnV("##ControlsButton", imgui.TableColumnFlagsWidthFixed, 48, 0)

	// Speed PID
	{
		w.tableRow("Speed PID:")
		pid := [4]float32{w.speedKp, w.speedKi, w.speedKd, w.speedAlpha}
		imgui.SetNextItemWidth(imgui.ContentRegionAvail().X - 48)
		if imgui.InputFloat4V("##SpeedPID", &pid, "%.4f", 0) {
			w.speedKp, w.speedKi, w.speedKd, w.speedAlpha = pid[0], pid[1], pid[2], pid[3]
		}
		imgui.TableSetColumnIndex(2)
		if w.toolbarButton("##SpeedPIDSend", "play-black") {
			w.mouse.SendCommand(mouse.NewTunePIDCommand(mouse.SpeedPid, w.speedKp, w.speedKi, w.speedKd, w.speedAlpha))
		}
	}

	// Wall PID
	{
		w.tableRow("Wall PID:")
		pid := [4]float32{w.wallKp, w.wallKi, w.wallKd, w.wallAlpha}
		imgui.SetNextItemWidth(imgui.ContentRegionAvail().X - 48)
		if imgui.InputFloat4V("##WallPID", &pid, "%.4f", 0) {
			w.wallKp, w.wallKi, w.wallKd, w.wallAlpha = pid[0], pid[1], pid[2], pid[3]
		}
		imgui.TableSetColumnIndex(2)
		if w.toolbarButton("##WallPIDSend", "play-black") {
			w.mouse.SendCommand(mouse.NewTunePIDCommand(mouse.WallPid, w.wallKp, w.wallKi, w.wallKd, w.wallAlpha))
		}
	}

	// Angle PID
	{
		w.tableRow("Angle PID:")
		pid := [4]float32{w.angleKp, w.angleKi, w.angleKd, w.angleAlpha}
		imgui.SetNextItemWidth(imgui.ContentRegionAvail().X - 48)
		if imgui.InputFloat4V("##AnglePID", &pid, "%.4f", 0) {
			w.angleKp, w.angleKi, w.angleKd, w.angleAlpha = pid[0], pid[1], pid[2], pid[3]
		}
		imgui.TableSetColumnIndex(2)
		if w.toolbarButton("##AnglePIDSend", "play-black") {
			w.mouse.SendCommand(mouse.NewTunePIDCommand(mouse.AnglePid, w.angleKp, w.angleKi, w.angleKd, w.angleAlpha))
		}
	}

	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Separator()
	imgui.TableSetColumnIndex(1)
	imgui.Separator()
	imgui.TableSetColumnIndex(2)
	imgui.Separator()

	// LEDs
	{
		w.tableRow("Status LEDs:")
		w.drawIconToggleButton("##LeftLED", "led-off-white", "led-on-lightblue", &w.ledLeft)
		imgui.SameLineV(0, 20)
		w.drawIconToggleButton("##OnboardLED", "led-off-white", "led-on-orange", &w.ledBuiltin)
		imgui.SameLineV(0, 20)
		w.drawIconToggleButton("##RightLED", "led-off-white", "led-on-lightblue", &w.ledRight)
		imgui.TableSetColumnIndex(2)
		if w.toolbarButton("##LEDPlan", "plus-thick") {
			w.mouse.SendCommand(mouse.NewEnqueuePlanCommand(
				mouse.PlanLEDs{Left: w.ledLeft, Builtin: w.ledBuiltin, Right: w.ledRight},
			))
		}
	}

	// IR
	{
		w.tableRow("IR LEDs:")
		w.drawIconToggleButton("##IR1", "led-off-white", "led-on-red", &w.ledIR)
		imgui.SameLineV(0, 20)
		w.drawIconToggleButton("##IR2", "led-off-white", "led-on-red", &w.ledIR)
		imgui.SameLineV(0, 20)
		w.drawIconToggleButton("##IR3", "led-off-white", "led-on-red", &w.ledIR)
		imgui.TableSetColumnIndex(2)
		if w.toolbarButton("##IRPlan", "plus-thick") {
			w.mouse.SendCommand(mouse.NewEnqueuePlanCommand(
				mouse.PlanIR{On: w.ledIR},
			))
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
			w.mouse.SendCommand(mouse.NewEnqueuePlanCommand(
				mouse.PlanFixedPower{Left: int16(w.powerLeft), Right: int16(w.powerRight)},
			))
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
			w.mouse.SendCommand(mouse.NewEnqueuePlanCommand(
				mouse.PlanFixedSpeed{Left: w.speedLeft, Right: w.speedRight},
			))
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
			w.mouse.SendCommand(mouse.NewEnqueuePlanCommand(
				mouse.PlanLinearMotion{Distance: w.linearDistance, Stop: w.linearStop},
			))
		}
	}

	// Rotational
	{
		w.tableRow("Rotational:")
		imgui.InputFloat("°", &w.rotationalDTheta)
		imgui.SameLine()
		imgui.TableSetColumnIndex(2)
		if w.toolbarButton("##RotationalPlan", "plus-thick") {
			w.mouse.SendCommand(mouse.NewEnqueuePlanCommand(
				mouse.PlanRotationalMotion{DTheta: w.rotationalDTheta * math.Pi / 180},
			))
		}
	}

	// Sensor Cal
	{
		w.tableRow("Sensor Cal:")
		imgui.TableSetColumnIndex(2)
		if w.toolbarButton("##SensorCalPlan", "plus-thick") {
			w.mouse.SendCommand(mouse.NewEnqueuePlanCommand(
				mouse.PlanSensorCal{},
			))
		}
	}

	// Execute
	{
		w.tableRow("")
		imgui.TableSetColumnIndex(2)
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
