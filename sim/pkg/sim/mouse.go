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

import (
	"math"
	"sync"
)

// Fundamental constants
const (
	GridSize                float64 = 180.0 // in mm
	AxleOffset              float64 = 36.00 // in mm
	WheelBase               float64 = 72.48 // in mm
	WheelDiameter           float64 = 32.5  // in mm
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
	Pos       Pos
	Distance  float64
	WallAngle float64
}

type Mouse struct {
	avr                                   *C.avr_t
	maze                                  *Maze
	encoderChan                           <-chan EncoderTickEvent
	ledChan                               <-chan struct{}
	LEDs                                  *LEDs
	Battery                               *Battery
	FunctionSelector                      *FunctionSelector
	LeftSensor, CenterSensor, RightSensor *Sensor
	LeftMotor, RightMotor                 *Motor
	X, Y                                  float64 // center of the axle, in mm from the maze center
	Angle                                 float64 // in radians, 0 is the positive x-axis (east)
	irHits                                map[*Sensor]IRHit
	irMutex                               sync.Mutex
}

func NewMouse(avr *C.avr_t, maze *Maze) *Mouse {
	encoderChan := make(chan EncoderTickEvent, 100)
	ledChan := make(chan struct{}, 100)

	return &Mouse{
		avr:              avr,
		maze:             maze,
		encoderChan:      encoderChan,
		ledChan:          ledChan,
		LEDs:             NewLEDs(avr, ledChan),
		Battery:          NewBattery(avr, C.ADC_IRQ_ADC7),
		FunctionSelector: NewFunctionSelect(avr, C.ADC_IRQ_ADC6),
		LeftMotor:        NewMotor(avr, true, encoderChan),
		RightMotor:       NewMotor(avr, false, encoderChan),
		LeftSensor:       NewSensor(avr, "SENSOR_LEFT", C.ADC_IRQ_ADC2, Pos{X: 50, Y: 40}, math.Pi/3),
		CenterSensor:     NewSensor(avr, "SENSOR_CENTER", C.ADC_IRQ_ADC1, Pos{X: 65, Y: 0}, 0),
		RightSensor:      NewSensor(avr, "SENSOR_RIGHT", C.ADC_IRQ_ADC0, Pos{X: 50, Y: -40}, -math.Pi/3),
		X:                GridSize / 2,
		Y:                AxleOffset,
		Angle:            math.Pi / 2,
		irHits:           make(map[*Sensor]IRHit, 3),
		irMutex:          sync.Mutex{},
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
		for {
			select {
			case <-m.ledChan:
				m.updateIRHits()
			case ev := <-m.encoderChan:
				m.EncoderTick(ev.Left, ev.Left != ev.Clockwise)
			}
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
	var fudge float64
	if left {
		fudge = 1.0 + 0.00
	} else {
		fudge = 1.0 - 0.00
	}
	m.SetPosition(
		m.X+(m.dx(left, forward)*fudge),
		m.Y+(m.dy(left, forward)*fudge),
		math.Mod(m.Angle+m.dt(left, forward)*fudge, 2*math.Pi),
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

func (m *Mouse) IRHits() map[*Sensor]IRHit {
	m.irMutex.Lock()
	defer m.irMutex.Unlock()

	result := make(map[*Sensor]IRHit, len(m.irHits))
	for k, v := range m.irHits {
		result[k] = v
	}
	return result
}

func (m *Mouse) updateIRHits() {
	m.irMutex.Lock()
	defer m.irMutex.Unlock()

	for _, sensor := range []*Sensor{m.LeftSensor, m.CenterSensor, m.RightSensor} {
		bestHit := IRHit{Pos: Pos{}, Distance: math.Inf(1), WallAngle: 0}

		origin := m.mouse2Maze(sensor.Pos)
		angle := sensor.Angle + m.Angle

		if m.LEDs.IR {
			tanTheta := math.Tan(angle)
			sinSign := math.Signbit(math.Sin(angle))
			cosSign := math.Signbit(math.Cos(angle))

			// horizontal walls
			for y := 0; y < len(m.maze.Posts); y++ {
				wallY := float64(y) * GridSize
				dy := wallY - origin.Y
				dx := 0.0
				if tanTheta != 0 {
					dx = dy / tanTheta
				}
				if math.Signbit(dx) != cosSign {
					continue
				}
				x := origin.X + dx
				if x < 0 || x > (float64(len(m.maze.Posts[y]))*GridSize-1) {
					continue
				}
				if m.maze.Posts[y][int(x/GridSize)].East {
					d := math.Sqrt(dx*dx + dy*dy)
					if d < bestHit.Distance {
						bestHit.Pos = Pos{x, wallY}
						bestHit.Distance = d
						bestHit.WallAngle = 0
					}
				}
			}

			// vertical walls
			for x := 0; x < len(m.maze.Posts[0]); x++ {
				wallX := float64(x) * GridSize
				dx := wallX - origin.X
				dy := dx * tanTheta
				if math.Signbit(dy) != sinSign {
					continue
				}
				y := origin.Y + dy
				if y < 0 || y > (float64(len(m.maze.Posts))*GridSize-1) {
					continue
				}
				if m.maze.Posts[int(y/GridSize)][x].North {
					d := math.Sqrt(dx*dx + dy*dy)
					if d < bestHit.Distance {
						bestHit.Pos = Pos{wallX, y}
						bestHit.Distance = d
						bestHit.WallAngle = math.Pi / 2
					}
				}
			}
		}

		m.irHits[sensor] = bestHit

		if bestHit.Distance == math.Inf(1) {
			sensor.Voltage = 0
		} else {
			angleScale := math.Abs(math.Sin(bestHit.WallAngle - angle))
			sensor.Voltage = int32(angleScale * math.Min(5000, 1200000.0/math.Pow(bestHit.Distance, 1000.0/583.0)))
		}
	}
}

func (m *Mouse) mouse2Maze(p Pos) Pos {
	return Pos{
		X: m.X + (p.X*math.Cos(m.Angle) - p.Y*math.Sin(m.Angle)),
		Y: m.Y + (p.X*math.Sin(m.Angle) + p.Y*math.Cos(m.Angle)),
	}
}
