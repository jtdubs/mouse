package ui

import (
	"fmt"
	"math"

	"github.com/jtdubs/mouse/remote/pkg/mouse"

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

func (w *reportWindow) draw() {
	imgui.Begin("Report")

	imgui.BeginTable("##Status", 2)
	imgui.TableSetupColumnV("##StatusLabel", imgui.TableColumnFlagsWidthFixed, 160, 0)
	imgui.TableSetupColumnV("##StatusControl", imgui.TableColumnFlagsWidthStretch, 0, 0)

	r := w.mouse.Report()

	{
		w.tableRow("Time:")
		imgui.Text(
			fmt.Sprintf(
				"%02d:%02d:%02d.%03d.%03d",
				(r.RTCMicros / 3600000000),
				(r.RTCMicros/60000000)%60,
				(r.RTCMicros/1000000)%60,
				(r.RTCMicros/1000)%1000,
				r.RTCMicros%1000,
			),
		)
	}

	w.tableRow("Status LEDs:")
	{
		onboard, left, right, ir := r.DecodeLEDs()
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
		w.tableRow("Plan:")
		imgui.Text(fmt.Sprintf("%v - %#v", mouse.PlanStateNames[r.Plan.State], r.Plan.Decode()))
	}

	{
		w.tableRow("Position:")
		measured := [2]float32{r.PositionDistance, r.PositionTheta * 180.0 / math.Pi}
		imgui.BeginDisabled()
		imgui.InputFloat2("##Position", &measured)
		imgui.EndDisabled()
	}

	{
		w.tableRow("Speed:")
		measured := [2]float32{r.SpeedMeasuredLeft, r.SpeedMeasuredRight}
		setpoint := [2]float32{r.SpeedSetpointLeft, r.SpeedSetpointRight}
		imgui.BeginDisabled()
		imgui.SliderFloat2("rpm (setpoint)##SpeedSetpoint", &setpoint, -200, 200)
		imgui.SliderFloat2("rpm (measured)##SpeedMeasured", &measured, -200, 200)
		imgui.EndDisabled()
	}

	{
		w.tableRow("Motor:")
		power := [2]int32{int32(r.MotorPowerLeft), int32(r.MotorPowerRight)}
		imgui.BeginDisabled()
		imgui.SliderInt2("##Motor", &power, -512, 512)
		imgui.EndDisabled()
	}

	{
		w.tableRow("Encoders:")
		encoders := [2]int32{r.EncoderLeft, r.EncoderRight}
		imgui.BeginDisabled()
		imgui.InputInt2("##Encoders", &encoders)
		imgui.EndDisabled()
	}

	{
		w.tableRow("Sensors:")
		l, c, r := r.DecodeSensors()
		sensors := [3]int32{int32(l), int32(c), int32(r)}
		imgui.BeginDisabled()
		imgui.SliderInt3("##Encoders", &sensors, 0, 1023)
		imgui.EndDisabled()
	}

	{
		height := imgui.ContentRegionAvail().Y / 2
		w.tableRow("Right PID:")
		if imgui.PlotBeginPlotV("Right##RightPlot", imgui.NewVec2(-1, height), imgui.PlotFlagsNoLegend|imgui.PlotFlagsNoTitle) {
			imgui.PlotSetupAxesLimits(0, 2000, 0, 500)
			imgui.PlotPlotLineFloatPtrIntV("PV##RightPV", w.mouse.RightSpeedMeasurements.Buffer(), 2000, 1, 0, 0, 0, 4)
			imgui.PlotPlotLineFloatPtrIntV("SP##RightPV", w.mouse.RightSpeedSetpoints.Buffer(), 2000, 1, 0, 0, 0, 4)
			imgui.PlotEndPlot()
		}
	}

	{
		height := imgui.ContentRegionAvail().Y
		w.tableRow("Left PID:")
		if imgui.PlotBeginPlotV("Left##LeftPlot", imgui.NewVec2(-1, height), imgui.PlotFlagsNoLegend|imgui.PlotFlagsNoTitle) {
			imgui.PlotSetupAxesLimits(0, 2000, 0, 500)
			imgui.PlotPlotLineFloatPtrIntV("PV##LeftPV", w.mouse.LeftSpeedMeasurements.Buffer(), 2000, 1, 0, 0, 0, 4)
			imgui.PlotPlotLineFloatPtrIntV("SP##LeftPV", w.mouse.LeftSpeedSetpoints.Buffer(), 2000, 1, 0, 0, 0, 4)
			imgui.PlotEndPlot()
		}
	}

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
