package ui

import (
	"fmt"

	"github.com/jtdubs/mouse/ui/pkg/mouse"

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

	// {
	// 	modes := []string{"Remote", "Wall Sensor", "Error", "Unknown #3", "Unknown #4", "Unknown #5", "Unknown #6", "Unknown #7"}
	// 	imgui.TableNextRow()
	// 	imgui.TableSetColumnIndex(0)
	// 	imgui.Text("Mode:")
	// 	imgui.TableSetColumnIndex(1)
	// 	if w.mouse.Open() {
	// 		imgui.Text(modes[r.Mode])
	// 	} else {
	// 		imgui.Text("Disconnected")
	// 	}
	// }

	// w.tableRow("Status LEDs:")
	// {
	// 	onboard, left, right, ir := r.DecodeLEDs()
	// 	if left {
	// 		w.drawIcon("led-on-lightblue")
	// 	} else {
	// 		w.drawIcon("led-off-white")
	// 	}
	// 	imgui.SameLineV(0, 20)
	// 	if onboard {
	// 		w.drawIcon("led-on-orange")
	// 	} else {
	// 		w.drawIcon("led-off-white")
	// 	}
	// 	imgui.SameLineV(0, 20)
	// 	if right {
	// 		w.drawIcon("led-on-lightblue")
	// 	} else {
	// 		w.drawIcon("led-off-white")
	// 	}

	// 	w.tableRow("IR LEDs:")
	// 	if ir {
	// 		w.drawIcon("led-on-red")
	// 		imgui.SameLineV(0, 20)
	// 		w.drawIcon("led-on-red")
	// 		imgui.SameLineV(0, 20)
	// 		w.drawIcon("led-on-red")
	// 	} else {
	// 		w.drawIcon("led-off-white")
	// 		imgui.SameLineV(0, 20)
	// 		w.drawIcon("led-off-white")
	// 		imgui.SameLineV(0, 20)
	// 		w.drawIcon("led-off-white")
	// 	}
	// }

	// {
	// 	w.tableRow("Position:")
	// 	measured := [2]float32{r.PositionMeasuredLeft, r.PositionMeasuredRight}
	// 	setpoint := [2]float32{r.PositionSetpointLeft, r.PositionSetpointRight}
	// 	imgui.BeginDisabled()
	// 	imgui.SliderFloat2("mm (setpoint)##PositionSetpoint", &setpoint, 0, 16.0*180.0)
	// 	imgui.SliderFloat2("mm (measured)##PositionMeasured", &measured, 0, 16.0*180.0)
	// 	imgui.EndDisabled()
	// }

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

	// {
	// 	w.tableRow("Encoders:")
	// 	power := [2]int32{int32(r.EncoderLeft), int32(r.EncoderRight)}
	// 	imgui.BeginDisabled()
	// 	imgui.SliderInt2("##Encoders", &power, 0, 16383)
	// 	imgui.EndDisabled()
	// }

	{
		w.tableRow("Sensors:")
		l, c, r := r.DecodeSensors()
		sensors := [3]int32{int32(l), int32(c), int32(r)}
		imgui.BeginDisabled()
		imgui.SliderInt3("##Encoders", &sensors, 0, 1023)
		imgui.EndDisabled()
	}

	{
		w.tableRow("Speed PID:")
		pid := [3]float32{r.SpeedKp, r.SpeedKi, r.SpeedKd}
		imgui.SliderFloat3("##SpeedPID", &pid, 0, 1)
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
