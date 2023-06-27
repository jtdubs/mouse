package sim

/*
#cgo pkg-config: simavr simavrparts

#include <avr_adc.h>
#include <avr_ioport.h>
#include <avr_timer.h>
#include <parts/uart_pty.h>
#include <sim_vcd_file.h>
#include <sim_avr.h>
#include <sim_elf.h>
#include <sim_gdb.h>
*/
import "C"
import "math"

// Fundamental constants
const (
	GridSize                float64 = 180.0 // in mm
	WheelBase               float64 = 90.0  // in mm
	WheelDiameter           float64 = 32.0  // in mm
	EncoderTicksPerRotation int     = 240
)

// Derived constants
var (
	WheelCircumference     float64 = math.Pi * WheelDiameter
	DistancePerEncoderTick float64 = WheelCircumference / float64(EncoderTicksPerRotation)
	EncoderTickDtheta      float64 = DistancePerEncoderTick / WheelBase
	EncoderTickDx          float64 = (WheelBase / 2.0) * math.Sin(EncoderTickDtheta)
	EncoderTickDy          float64 = (WheelBase / 2.0) * (1.0 - math.Cos(EncoderTickDtheta))
)

type Pos struct{ X, Y float64 }

type IRHit struct {
	Sensor *Sensor
	Pos    Pos
}

type Mouse struct {
	avr                                   *C.avr_t
	LEDs                                  *LEDs
	Battery                               *Battery
	FunctionSelector                      *FunctionSelector
	LeftSensor, CenterSensor, RightSensor *Sensor
	LeftMotor, RightMotor                 *Motor
	X, Y                                  float64 // center of the axle, in mm from the maze center
	Angle                                 float64 // in radians, 0 is the positive x-axis (east)
	encoderChan                           <-chan EncoderTickEvent
	IRHits                                []IRHit
	maze                                  *Maze
}

func NewMouse(avr *C.avr_t, maze *Maze) *Mouse {
	encoderChan := make(chan EncoderTickEvent, 100)

	return &Mouse{
		avr:              avr,
		LEDs:             NewLEDs(avr),
		Battery:          NewBattery(avr, C.ADC_IRQ_ADC7),
		FunctionSelector: NewFunctionSelect(avr, C.ADC_IRQ_ADC6),
		LeftMotor:        NewMotor(avr, true, encoderChan),
		RightMotor:       NewMotor(avr, false, encoderChan),
		LeftSensor:       NewSensor(avr, "SENSOR_LEFT", C.ADC_IRQ_ADC2, Pos{X: 50, Y: 35}),
		CenterSensor:     NewSensor(avr, "SENSOR_CENTER", C.ADC_IRQ_ADC1, Pos{X: 65, Y: 0}),
		RightSensor:      NewSensor(avr, "SENSOR_RIGHT", C.ADC_IRQ_ADC0, Pos{X: 50, Y: -35}),
		encoderChan:      encoderChan,
		X:                GridSize / 2,
		Y:                GridSize / 2,
		Angle:            math.Pi / 2,
	}
}

func (m *Mouse) Init() {
	m.LEDs.Init()
	m.Battery.Init()
	m.FunctionSelector.Init()
	m.LeftMotor.Init()
	m.RightMotor.Init()
	m.LeftSensor.Init()
	m.CenterSensor.Init()
	m.RightSensor.Init()

	go func() {
		for ev := range m.encoderChan {
			m.EncoderTick(ev.Left, ev.Left != ev.Clockwise)
		}
	}()
}

func (m *Mouse) SetPosition(x, y, angle float64) {
	m.X = x
	m.Y = y
	m.Angle = angle

	m.updateIRHits()
}

func (m *Mouse) EncoderTick(left, forward bool) {
	m.SetPosition(
		m.X+m.dx(left, forward),
		m.Y+m.dy(left, forward),
		math.Mod(m.Angle+m.dt(left, forward), 2*math.Pi),
	)
}

func (m *Mouse) dx(left, forward bool) float64 {
	dx := EncoderTickDx
	if !forward {
		dx = -dx
	}
	dy := EncoderTickDy
	if left {
		dy = -dy
	}
	return (dx * math.Cos(m.Angle)) - (dy * math.Sin(m.Angle))
}

func (m *Mouse) dy(left, forward bool) float64 {
	dx := EncoderTickDx
	if !forward {
		dx = -dx
	}
	dy := EncoderTickDy
	if left {
		dy = -dy
	}
	return (dx * math.Sin(m.Angle)) + (dy * math.Cos(m.Angle))
}

func (m *Mouse) dt(left, forward bool) float64 {
	if left != forward {
		return EncoderTickDtheta
	} else {
		return -EncoderTickDtheta
	}
}

func (m *Mouse) updateIRHits() {
	m.IRHits = []IRHit{
		{m.RightSensor, Pos{360, 360}},
	}
}
