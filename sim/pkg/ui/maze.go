package ui

import (
	"fmt"
	"math"
	"strings"

	"github.com/jtdubs/mouse/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

type mazeWindow struct {
	sim       *sim.Sim
	m         *sim.Mouse
	mazeList  string
	mazeIndex int32
}

func newMazeWindow(sim *sim.Sim) *mazeWindow {
	return &mazeWindow{
		sim: sim,
		m:   sim.Mouse,
	}
}

func (s *mazeWindow) init() {
	s.mazeList = strings.Join(s.sim.Maze.ListMazes(), "\x00") + "\x00\x00"
}

func (s *mazeWindow) draw() {
	imgui.Begin("Maze")

	imgui.Text("Maze:")
	imgui.SameLine()
	if imgui.ComboStr("##MAZE", &s.mazeIndex, s.mazeList) {
		s.sim.Maze.Load(s.sim.Maze.ListMazes()[s.mazeIndex])
	}
	imgui.SameLine()
	imgui.Text(fmt.Sprintf("(x=%02.2f, y=%02.2f, θ=%02.2f)", s.m.Environment.MouseX, s.m.Environment.MouseY, s.m.Environment.MouseAngle*180.0/math.Pi))

	canvasSizePx := imgui.ContentRegionAvail()
	postDim := imgui.NewVec2(float32(len(s.sim.Maze.Posts[0])), float32(len(s.sim.Maze.Posts)))
	gridDim := postDim.Sub(imgui.NewVec2(1, 1))
	cellSizePx := float32(math.Min(float64(canvasSizePx.X/gridDim.X), float64(canvasSizePx.Y/gridDim.Y)))
	mazeSizePx := imgui.NewVec2(cellSizePx*gridDim.X, cellSizePx*gridDim.Y)
	mmSizePx := cellSizePx / 180.0
	marginPx := canvasSizePx.Sub(mazeSizePx).Div(2.0)
	mazeOriginPx := imgui.CursorScreenPos().Add(imgui.NewVec2(0, canvasSizePx.Y)).Add(imgui.NewVec2(marginPx.X, -marginPx.Y))

	drawList := imgui.WindowDrawList()
	for i, row := range s.sim.Maze.Posts {
		for j, post := range row {
			cellOriginPx := mazeOriginPx.Add(imgui.NewVec2(float32(j)*cellSizePx, -float32(i)*cellSizePx))
			if post.North {
				drawList.AddLineV(cellOriginPx, cellOriginPx.Add(imgui.NewVec2(0, -cellSizePx)), imgui.ColorConvertFloat4ToU32(imgui.NewVec4(1, 1, 1, 1)), 2.0)
			}
			if post.East {
				drawList.AddLineV(cellOriginPx, cellOriginPx.Add(imgui.NewVec2(cellSizePx, 0)), imgui.ColorConvertFloat4ToU32(imgui.NewVec4(1, 1, 1, 1)), 2.0)
			}
		}
	}

	x, y, theta := float32(s.m.Environment.MouseX), float32(s.m.Environment.MouseY), s.m.Environment.MouseAngle
	centerPx := mazeOriginPx.Add(imgui.NewVec2(x, -y).Mul(mmSizePx))

	// 65mm in front of wheels, 35mm behind
	// axle width is 90mm

	mouseXY := func(mm imgui.Vec2) (px imgui.Vec2) {
		deltaPx := mm.Mul(mmSizePx)
		return centerPx.Add(
			imgui.NewVec2(
				deltaPx.X*float32(math.Cos(theta))-deltaPx.Y*float32(math.Sin(theta)),
				-(deltaPx.X*float32(math.Sin(theta)) + deltaPx.Y*float32(math.Cos(theta))),
			))
	}

	outline := []imgui.Vec2{
		mouseXY(imgui.NewVec2(0, 45)),
		mouseXY(imgui.NewVec2(50, 45)),
		mouseXY(imgui.NewVec2(65, 30)),
		mouseXY(imgui.NewVec2(65, -30)),
		mouseXY(imgui.NewVec2(50, -45)),
		mouseXY(imgui.NewVec2(-35, -45)),
		mouseXY(imgui.NewVec2(-35, 45)),
	}

	for i := range outline {
		drawList.AddLineV(outline[i], outline[(i+1)%len(outline)], imgui.ColorConvertFloat4ToU32(imgui.NewVec4(0.5, 1, 0.5, 1)), 2.0)
	}
	drawList.AddLineV(mouseXY(imgui.NewVec2(0, 45)), mouseXY(imgui.NewVec2(0, -45)), imgui.ColorConvertFloat4ToU32(imgui.NewVec4(0.5, 1, 0.5, 1)), 2.0)

	imgui.End()
}
