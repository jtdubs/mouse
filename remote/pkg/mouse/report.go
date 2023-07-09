package mouse

import (
	"bytes"
	"encoding/binary"
	"fmt"
	"unsafe"

	"github.com/elamre/vcd"
)

type ReportKey uint8

const (
	ReportKeyLatest ReportKey = iota
	ReportKeyPlatform
	ReportKeyControl
	ReportKeyUnknown
)

type ReportType uint8

const (
	ReportTypePlatform ReportType = 1
	ReportTypeControl  ReportType = 2
	// ReportTypeMaze
)

type Report struct {
	Header ReportHeader
	Body   ReportBody
}

func (r Report) Key() ReportKey {
	if r.Body == nil {
		return ReportKeyUnknown
	}
	return r.Body.Key()
}

type ReportHeader struct {
	Type      ReportType
	RTCMicros uint32
}

type ReportBody interface {
	Symbols() map[string]string
	Key() ReportKey
}

type PlatformReport struct {
	BatteryVolts              uint8
	Sensors                   uint32
	LEDs                      uint8
	LeftEncoder, RightEncoder int32
	LeftMotor, RightMotor     int16
}

func (r PlatformReport) Key() ReportKey {
	return ReportKeyPlatform
}

func (r PlatformReport) DecodeSensors() (left, center, right uint16) {
	left = uint16(r.Sensors & 0x3ff)
	center = uint16((r.Sensors >> 10) & 0x3ff)
	right = uint16((r.Sensors >> 20) & 0x3ff)
	return
}

func (r PlatformReport) DecodeLEDs() (onboard, left, right, ir bool) {
	onboard = (r.LEDs & 0x01) == 1
	left = ((r.LEDs >> 1) & 0x01) == 1
	right = ((r.LEDs >> 2) & 0x01) == 1
	ir = ((r.LEDs >> 3) & 0x01) == 1
	return
}

type ControlReport struct {
	Header ControlReportHeader
	Body   ControlReportBody
}

func (r ControlReport) Key() ReportKey {
	return ReportKeyControl
}

type ControlReportHeader struct {
	Plan               EncodedPlan
	SpeedMeasuredLeft  float32
	SpeedMeasuredRight float32
	SpeedSetpointLeft  float32
	SpeedSetpointRight float32
	PositionDistance   float32
	PositionTheta      float32
}

type ControlReportBody interface {
	Symbols(p DecodedPlan) map[string]string
	isControlReport()
}

type SensorCalReport struct {
	Left, Right, Center uint16
}

func (SensorCalReport) isControlReport() {}

type RotationReport struct {
	StartTheta, TargetTheta float32
	Direction               bool
}

func (RotationReport) isControlReport() {}

type LinearReport struct {
	StartDistance, TargetDistance, TargetSpeed float32
}

func (LinearReport) isControlReport() {}

func ReadReport(r *bytes.Reader) (Report, error) {
	var header ReportHeader
	if err := binary.Read(r, binary.LittleEndian, &header); err != nil {
		return Report{}, err
	}

	var (
		body ReportBody
		err  error
	)
	switch header.Type {
	case ReportTypePlatform:
		if body, err = ReadPlatformReport(r); err != nil {
			return Report{}, err
		}
	case ReportTypeControl:
		if body, err = ReadControlReport(r); err != nil {
			return Report{}, err
		}
	default:
		return Report{}, fmt.Errorf("unknown report type: %v", header.Type)
	}

	return Report{
		Header: header,
		Body:   body,
	}, nil
}

func ReadPlatformReport(r *bytes.Reader) (PlatformReport, error) {
	var report PlatformReport
	err := binary.Read(r, binary.LittleEndian, &report)
	return report, err
}

func ReadControlReport(r *bytes.Reader) (ControlReport, error) {
	var header ControlReportHeader
	if err := binary.Read(r, binary.LittleEndian, &header); err != nil {
		return ControlReport{}, err
	}

	var body ControlReportBody
	switch header.Plan.Decode().planType() {
	case PlanTypeLinearMotion:
		var report LinearReport
		if err := binary.Read(r, binary.LittleEndian, &report); err != nil {
			return ControlReport{}, err
		}
		body = report
	case PlanTypeRotationalMotion:
		var report RotationReport
		if err := binary.Read(r, binary.LittleEndian, &report); err != nil {
			return ControlReport{}, err
		}
		body = report
	case PlanTypeSensorCal:
		var report SensorCalReport
		if err := binary.Read(r, binary.LittleEndian, &report); err != nil {
			return ControlReport{}, err
		}
		body = report
	default:
		body = nil
	}

	return ControlReport{
		Header: header,
		Body:   body,
	}, nil
}

func (Report) Variables() []vcd.VcdDataType {
	result := []vcd.VcdDataType{
		vcd.NewVariable("rtc_now", "wire", 32),
	}
	result = append(result, PlatformReport{}.Variables()...)
	result = append(result, ControlReport{}.Variables()...)
	return result
}

func (PlatformReport) Variables() []vcd.VcdDataType {
	return []vcd.VcdDataType{
		vcd.NewVariable("platform_battery_volts", "wire", 8),
		vcd.NewVariable("platform_sensor_left", "wire", 10),
		vcd.NewVariable("platform_sensor_center", "wire", 10),
		vcd.NewVariable("platform_sensor_right", "wire", 10),
		vcd.NewVariable("platform_led_left", "wire", 1),
		vcd.NewVariable("platform_led_right", "wire", 1),
		vcd.NewVariable("platform_led_onboard", "wire", 1),
		vcd.NewVariable("platform_led_ir", "wire", 1),
		vcd.NewVariable("platform_encoder_left", "wire", 32),
		vcd.NewVariable("platform_encoder_right", "wire", 32),
		vcd.NewVariable("platform_motor_left", "wire", 16),
		vcd.NewVariable("platform_motor_right", "wire", 16),
	}
}

func (ControlReport) Variables() []vcd.VcdDataType {
	result := []vcd.VcdDataType{
		vcd.NewVariable("control_plan_type", "wire", 8),
		vcd.NewVariable("control_plan_state", "wire", 8),
		vcd.NewVariable("control_speed_measured_left", "wire", 32),
		vcd.NewVariable("control_speed_measured_right", "wire", 32),
		vcd.NewVariable("control_speed_setpoint_left", "wire", 32),
		vcd.NewVariable("control_speed_setpoint_right", "wire", 32),
		vcd.NewVariable("control_position_distance", "wire", 32),
		vcd.NewVariable("control_position_theta", "wire", 32),
	}
	result = append(result, LinearReport{}.Variables()...)
	result = append(result, RotationReport{}.Variables()...)
	result = append(result, SensorCalReport{}.Variables()...)
	return result
}

func (LinearReport) Variables() []vcd.VcdDataType {
	return []vcd.VcdDataType{
		vcd.NewVariable("linear_plan_distance", "wire", 32),
		vcd.NewVariable("linear_plan_stop", "wire", 1),
		vcd.NewVariable("linear_start_distnace", "wire", 32),
		vcd.NewVariable("linear_target_distance", "wire", 32),
		vcd.NewVariable("linear_target_speed", "wire", 32),
	}
}

func (RotationReport) Variables() []vcd.VcdDataType {
	return []vcd.VcdDataType{
		vcd.NewVariable("rotation_plan_dtheta", "wire", 32),
		vcd.NewVariable("rotation_start_dtheta", "wire", 32),
		vcd.NewVariable("rotation_target_dtheta", "wire", 32),
		vcd.NewVariable("rotation_direction", "wire", 1),
	}
}

func (SensorCalReport) Variables() []vcd.VcdDataType {
	return []vcd.VcdDataType{
		vcd.NewVariable("sensor_cal_left", "wire", 10),
		vcd.NewVariable("sensor_cal_center", "wire", 10),
		vcd.NewVariable("sensor_cal_right", "wire", 10),
	}
}

func (r Report) Symbols() map[string]string {
	result := map[string]string{
		"rtc_now": fmt.Sprint(r.Header.RTCMicros),
	}

	for k, v := range r.Body.Symbols() {
		result[k] = v
	}

	return result
}

func (r PlatformReport) Symbols() map[string]string {
	left, center, right := r.DecodeSensors()
	onboard, leftLED, rightLED, irLED := r.DecodeLEDs()

	return map[string]string{
		"platform_battery_volts": fmt.Sprint(r.BatteryVolts),
		"platform_sensor_left":   fmt.Sprint(left),
		"platform_sensor_center": fmt.Sprint(center),
		"platform_sensor_right":  fmt.Sprint(right),
		"platform_led_left":      boolMap[leftLED],
		"platform_led_right":     boolMap[rightLED],
		"platform_led_onboard":   boolMap[onboard],
		"platform_led_ir":        boolMap[irLED],
		"platform_encoder_left":  fmt.Sprint(r.LeftEncoder),
		"platform_encoder_right": fmt.Sprint(r.RightEncoder),
		"platform_motor_left":    fmt.Sprint(r.LeftMotor),
		"platform_motor_right":   fmt.Sprint(r.RightMotor),
	}
}

func (r ControlReport) Symbols() map[string]string {
	result := map[string]string{
		"control_plan_type":            fmt.Sprint(uint8(r.Header.Plan.Type)),
		"control_plan_state":           fmt.Sprint(uint8(r.Header.Plan.State)),
		"control_speed_measured_left":  fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.Header.SpeedMeasuredLeft))),
		"control_speed_measured_right": fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.Header.SpeedMeasuredRight))),
		"control_speed_setpoint_left":  fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.Header.SpeedSetpointLeft))),
		"control_speed_setpoint_right": fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.Header.SpeedSetpointRight))),
		"control_position_distance":    fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.Header.PositionDistance))),
		"control_position_theta":       fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.Header.PositionTheta))),
	}
	for k, v := range r.Body.Symbols(r.Header.Plan.Decode()) {
		result[k] = v
	}
	return result
}

func (r LinearReport) Symbols(p DecodedPlan) map[string]string {
	plan := p.(PlanLinearMotion)
	return map[string]string{
		"linear_plan_distance":   fmt.Sprint(*(*uint32)(unsafe.Pointer(&plan.Distance))),
		"linear_plan_stop":       boolMap[plan.Stop],
		"linear_start_distance":  fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.StartDistance))),
		"linear_target_distance": fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.TargetDistance))),
		"linear_target_speed":    fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.TargetSpeed))),
	}
}

func (r RotationReport) Symbols(p DecodedPlan) map[string]string {
	plan := p.(PlanRotationalMotion)
	return map[string]string{
		"rotation_plan_dtheta":   fmt.Sprint(*(*uint32)(unsafe.Pointer(&plan.DTheta))),
		"rotation_start_dtheta":  fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.StartTheta))),
		"rotation_target_dtheta": fmt.Sprint(*(*uint32)(unsafe.Pointer(&r.TargetTheta))),
		"rotation_direction":     boolMap[r.Direction],
	}
}

func (r SensorCalReport) Symbols(_ DecodedPlan) map[string]string {
	return map[string]string{
		"sensor_cal_left":   fmt.Sprint(r.Left),
		"sensor_cal_center": fmt.Sprint(r.Center),
		"sensor_cal_right":  fmt.Sprint(r.Right),
	}
}

var boolMap = map[bool]string{
	true:  "1",
	false: "0",
}
