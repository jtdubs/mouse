package ui

import (
	"fmt"
	"math"

	"github.com/jtdubs/mouse/tools/remote/pkg/mouse"

	imgui "github.com/AllenDang/cimgui-go"
)

type reportWindow struct {
	mouse *mouse.Mouse
}

func newReportWindow(mouse *mouse.Mouse) *reportWindow {
	return &reportWindow{
		mouse: mouse,
	}
}

func (w *reportWindow) init() {}

func (w *reportWindow) drawLatest() {
	r, ok := w.mouse.Report(mouse.ReportKeyLatest)
	if !ok {
		return
	}
	now := r.Header.RTCMicros

	w.tableRow("Time:")
	imgui.Text(
		fmt.Sprintf(
			"%02d:%02d:%02d.%03d.%03d",
			(now / 3600000000),
			(now/60000000)%60,
			(now/1000000)%60,
			(now/1000)%1000,
			now%1000,
		),
	)
}

func (w *reportWindow) drawPlatform() {
	report, ok := w.mouse.Report(mouse.ReportKeyPlatform)
	if !ok {
		return
	}
	r, ok := report.Body.(mouse.PlatformReport)
	if !ok {
		return
	}

	imgui.BeginDisabled()

	w.tableRow("Battery:")
	imgui.Text(fmt.Sprintf("%.2f V", float32(r.BatteryVolts)/100.0))

	w.tableRow("Status LEDs:")
	{
		left, right, onboard, ir := r.DecodeLEDs()
		if left {
			w.drawIcon("led-on-lightblue")
		} else {
			w.drawIcon("led-off-white")
		}
		imgui.SameLineV(0, 20)
		if onboard {
			w.drawIcon("led-on-orange")
		} else {
			w.drawIcon("led-off-white")
		}
		imgui.SameLineV(0, 20)
		if right {
			w.drawIcon("led-on-lightblue")
		} else {
			w.drawIcon("led-off-white")
		}

		w.tableRow("IR LEDs:")
		if ir {
			w.drawIcon("led-on-red")
			imgui.SameLineV(0, 20)
			w.drawIcon("led-on-red")
			imgui.SameLineV(0, 20)
			w.drawIcon("led-on-red")
		} else {
			w.drawIcon("led-off-white")
			imgui.SameLineV(0, 20)
			w.drawIcon("led-off-white")
			imgui.SameLineV(0, 20)
			w.drawIcon("led-off-white")
		}
	}

	{
		w.tableRow("Motor:")
		power := [2]int32{int32(r.LeftMotor), int32(r.RightMotor)}
		imgui.SliderInt2("##Motor", &power, -512, 512)
	}

	{
		w.tableRow("Encoders:")
		encoders := [2]int32{r.LeftEncoder, r.RightEncoder}
		imgui.InputInt2("##Encoders", &encoders)
	}

	{
		w.tableRow("Sensors:")
		l, c, r := r.DecodeSensors()
		sensors := [3]int32{int32(l), int32(c), int32(r)}
		imgui.SliderInt3("##Encoders", &sensors, 0, 1023)
	}

	imgui.EndDisabled()
}

func (w *reportWindow) drawControl() {
	report, ok := w.mouse.Report(mouse.ReportKeyControl)
	if !ok {
		return
	}
	r, ok := report.Body.(mouse.ControlReport)
	if !ok {
		return
	}

	imgui.BeginDisabled()

	{
		w.tableRow("Plan:")
		imgui.Text(fmt.Sprintf("%v - %#v", mouse.PlanStateNames[r.Header.Plan.State], r.Header.Plan.Decode()))
	}

	{
		w.tableRow("Position:")
		measured := [2]float32{r.Header.PositionDistance, r.Header.PositionTheta * 180.0 / math.Pi}
		imgui.InputFloat2("##Position", &measured)
	}

	{
		w.tableRow("Speed:")
		measured := [2]float32{r.Header.SpeedMeasuredLeft, r.Header.SpeedMeasuredRight}
		setpoint := [2]float32{r.Header.SpeedSetpointLeft, r.Header.SpeedSetpointRight}
		imgui.SliderFloat2("rpm (setpoint)##SpeedSetpoint", &setpoint, -200, 200)
		imgui.SliderFloat2("rpm (measured)##SpeedMeasured", &measured, -200, 200)
	}

	switch b := r.Body.(type) {
	case mouse.SensorCalReport:
		w.tableRow("Sensor Calibration:")
		values := [3]int32{int32(b.Left), int32(b.Right), int32(b.Forward)}
		imgui.InputInt3("##Sensors", &values)

	case mouse.RotationReport:
		w.tableRow("Rotation:")
		values := [2]float32{b.StartTheta, b.TargetTheta}
		imgui.InputFloat2("##Rotation", &values)

	case mouse.LinearReport:
		w.tableRow("Linear:")
		values := [3]float32{b.TargetDistance, b.TargetSpeed, b.WallError}
		imgui.InputFloat3("##Linear", &values)
	}

	imgui.EndDisabled()
}

func (w *reportWindow) draw() {
	if !imgui.Begin("Report") {
		return
	}

	imgui.BeginTable("##Status", 2)
	imgui.TableSetupColumnV("##StatusLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##StatusControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	w.drawLatest()
	w.drawPlatform()
	w.drawControl()

	imgui.EndTable()

	imgui.End()
}

func (w *reportWindow) tableRow(label string) {
	imgui.TableNextRow()
	imgui.TableSetColumnIndex(0)
	imgui.Text(label)
	imgui.TableSetColumnIndex(1)
}

func (w *reportWindow) drawIcon(name string) {
	imgui.ImageV(Textures[name].ID(), imgui.NewVec2(24, 24), imgui.NewVec2(0, 0), imgui.NewVec2(1, 1), imgui.NewVec4(1, 1, 1, 1), imgui.NewVec4(0, 0, 0, 0))
}
