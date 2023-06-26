package sim

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

type Environment struct {
	MouseX, MouseY float64 // center of the axle, in mm from the maze center
	MouseAngle     float64 // in radians, 0 is the positive x-axis (east)
	encoderChan    <-chan EncoderTickEvent
}

func NewEnvironment(mazeWidth, mazeHeight int, encoderChan <-chan EncoderTickEvent) *Environment {
	// start in the southwest corner, facing north
	return &Environment{
		MouseX:      GridSize / 2.0,
		MouseY:      GridSize / 2.0,
		MouseAngle:  math.Pi / 2.0,
		encoderChan: encoderChan,
	}
}

func (e *Environment) Init() {
	go func() {
		for ev := range e.encoderChan {
			e.EncoderTick(ev.Left, ev.Left != ev.Clockwise)
		}
	}()
}

func (e *Environment) dx(left, forward bool) float64 {
	dx := EncoderTickDx
	if !forward {
		dx = -dx
	}
	dy := EncoderTickDy
	if left {
		dy = -dy
	}
	return (dx * math.Cos(e.MouseAngle)) - (dy * math.Sin(e.MouseAngle))
}

func (e *Environment) dy(left, forward bool) float64 {
	dx := EncoderTickDx
	if !forward {
		dx = -dx
	}
	dy := EncoderTickDy
	if left {
		dy = -dy
	}
	return (dx * math.Sin(e.MouseAngle)) + (dy * math.Cos(e.MouseAngle))
}

func (e *Environment) dt(left, forward bool) float64 {
	if left != forward {
		return EncoderTickDtheta
	} else {
		return -EncoderTickDtheta
	}
}

func (e *Environment) EncoderTick(left, forward bool) {
	e.MouseX += e.dx(left, forward)
	e.MouseY += e.dy(left, forward)
	e.MouseAngle += e.dt(left, forward)
	if e.MouseAngle < 0 {
		e.MouseAngle += 2 * math.Pi
	} else if e.MouseAngle >= 2*math.Pi {
		e.MouseAngle -= 2 * math.Pi
	}
}
