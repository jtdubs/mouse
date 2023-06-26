package ui

import (
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
	postDim := imgui.NewVec2(float32(len(s.sim.Maze.Posts[0])), float32(len(s.sim.Maze.Posts)))
	gridDim := postDim.Sub(imgui.NewVec2(1, 1))

	imgui.Begin("Maze")

	imgui.Text("Maze:")
	imgui.SameLine()
	if imgui.ComboStr("##MAZE", &s.mazeIndex, s.mazeList) {
		s.sim.Maze.Load(s.sim.Maze.ListMazes()[s.mazeIndex])
	}

	canvasSizePx := imgui.ContentRegionAvail()
	cellSizePx := float32(math.Min(float64(canvasSizePx.X/gridDim.X), float64(canvasSizePx.Y/gridDim.Y)))
	mazeSizePx := imgui.NewVec2(cellSizePx*gridDim.X, cellSizePx*gridDim.Y)
	mmSizePx := cellSizePx / 180.0
	marginPx := canvasSizePx.Sub(mazeSizePx).Div(2.0)
	mazeOriginPx := imgui.CursorScreenPos().Add(imgui.NewVec2(0, canvasSizePx.Y)).Add(imgui.NewVec2(marginPx.X, -marginPx.Y))

	imgui.Dummy(canvasSizePx)
	if imgui.BeginPopupContextItem() {
		if imgui.SelectableBool("Reset Position") {
			s.m.Environment.MouseX, s.m.Environment.MouseY, s.m.Environment.MouseAngle = 90, 90, math.Pi/2
		}
		imgui.EndPopup()
	}

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

	minX, minY, maxX, maxY := float32(math.Inf(1)), float32(math.Inf(1)), float32(math.Inf(-1)), float32(math.Inf(-1))
	for i := range outline {
		if outline[i].X < minX {
			minX = outline[i].X
		}
		if outline[i].X > maxX {
			maxX = outline[i].X
		}
		if outline[i].Y < minY {
			minY = outline[i].Y
		}
		if outline[i].Y > maxY {
			maxY = outline[i].Y
		}
		drawList.AddLineV(outline[i], outline[(i+1)%len(outline)], imgui.ColorConvertFloat4ToU32(imgui.NewVec4(0.5, 1, 0.5, 1)), 2.0)
	}
	drawList.AddLineV(mouseXY(imgui.NewVec2(0, 45)), mouseXY(imgui.NewVec2(0, -45)), imgui.ColorConvertFloat4ToU32(imgui.NewVec4(0.5, 1, 0.5, 1)), 2.0)

	if dragging {
		imgui.SetCursorScreenPos(imgui.NewVec2(dragButtonX, dragButtonY))
	} else {
		imgui.SetCursorScreenPos(imgui.NewVec2(minX, minY))
	}
	imgui.InvisibleButton("##mouse", imgui.NewVec2(maxX-minX, maxY-minY))
	if imgui.IsItemHovered() {
		wheel := float64(imgui.CurrentIO().MouseWheel())
		if wheel != 0 {
			angle := s.m.Environment.MouseAngle * 180.0 / math.Pi
			angle = math.Round(angle/5.0) * 5.0
			angle += 5.0 * wheel
			s.m.Environment.MouseAngle = angle * math.Pi / 180.0
		}
	}
	if imgui.IsItemActive() {
		if !dragging {
			dragStartX, dragStartY = s.m.Environment.MouseX, s.m.Environment.MouseY
			dragButtonX, dragButtonY = minX, minY
			dragging = true
		} else {
			delta := imgui.MouseDragDeltaV(imgui.MouseButtonLeft, 5.0).Mul(1.0 / mmSizePx)
			s.m.Environment.MouseX = dragStartX + float64(delta.X)
			s.m.Environment.MouseY = dragStartY - float64(delta.Y)
		}
	} else {
		dragging = false
	}

	imgui.End()
}

var dragging = false
var dragStartX, dragStartY float64
var dragButtonX, dragButtonY float32