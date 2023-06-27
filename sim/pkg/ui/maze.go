package ui

import (
	"math"
	"strings"

	"github.com/jtdubs/mouse/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

type mazeWindow struct {
	sim       *sim.Sim
	mouse     *sim.Mouse
	mazeList  string
	mazeIndex int32
}

func newMazeWindow(sim *sim.Sim) *mazeWindow {
	return &mazeWindow{
		sim:   sim,
		mouse: sim.Mouse,
	}
}

func (w *mazeWindow) init() {
	w.mazeList = strings.Join(w.sim.Maze.ListMazes(), "\x00") + "\x00\x00"
}

func (w *mazeWindow) draw() {
	postDim := imgui.NewVec2(float32(len(w.sim.Maze.Posts[0])), float32(len(w.sim.Maze.Posts)))
	gridDim := postDim.Sub(imgui.NewVec2(1, 1))

	imgui.Begin("Maze")

	imgui.Text("Maze:")
	imgui.SameLine()
	if imgui.ComboStr("##MAZE", &w.mazeIndex, w.mazeList) {
		w.sim.Maze.Load(w.sim.Maze.ListMazes()[w.mazeIndex])
	}

	canvasSizePx := imgui.ContentRegionAvail()
	cellSizePx := float32(math.Min(float64(canvasSizePx.X/gridDim.X), float64(canvasSizePx.Y/gridDim.Y)))
	mazeSizePx := imgui.NewVec2(cellSizePx*gridDim.X, cellSizePx*gridDim.Y)
	mmSizePx := cellSizePx / float32(sim.GridSize)
	marginPx := canvasSizePx.Sub(mazeSizePx).Div(2.0)
	mazeOriginPx := imgui.CursorScreenPos().Add(imgui.NewVec2(0, canvasSizePx.Y)).Add(imgui.NewVec2(marginPx.X, -marginPx.Y))

	imgui.Dummy(canvasSizePx)
	if imgui.BeginPopupContextItem() {
		if imgui.SelectableBool("Reset Position") {
			w.mouse.SetPosition(sim.GridSize/2, sim.GridSize/2, math.Pi/2)
		}
		if imgui.SelectableBool("Center in Cell") {
			w.mouse.SetPosition(
				(math.Floor(w.mouse.X/sim.GridSize)+0.5)*sim.GridSize,
				(math.Floor(w.mouse.Y/sim.GridSize)+0.5)*sim.GridSize,
				math.Pi/2,
			)
		}
		imgui.EndPopup()
	}

	drawList := imgui.WindowDrawList()
	for i, row := range w.sim.Maze.Posts {
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

	x, y, theta := float32(w.mouse.X), float32(w.mouse.Y), w.mouse.Angle
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
			angle := w.mouse.Angle * 180.0 / math.Pi
			angle = math.Round(angle/5.0) * 5.0
			angle += 5.0 * wheel
			w.mouse.Angle = angle * math.Pi / 180.0
		}
	}
	if imgui.IsItemActive() {
		if !dragging {
			dragStartX, dragStartY = w.mouse.X, w.mouse.Y
			dragButtonX, dragButtonY = minX, minY
			dragging = true
		} else {
			delta := imgui.MouseDragDeltaV(imgui.MouseButtonLeft, 5.0).Mul(1.0 / mmSizePx)
			w.mouse.SetPosition(
				dragStartX+float64(delta.X),
				dragStartY-float64(delta.Y),
				w.mouse.Angle,
			)
		}
	} else {
		dragging = false
		w.mouse.SetPosition(
			math.Min(math.Max(w.mouse.X, 0), sim.GridSize*float64(gridDim.X)),
			math.Min(math.Max(w.mouse.Y, 0), sim.GridSize*float64(gridDim.Y)),
			w.mouse.Angle,
		)
	}

	for _, hit := range w.mouse.IRHits {
		s := mouseXY(imgui.NewVec2(float32(hit.Sensor.Pos.X), float32(hit.Sensor.Pos.Y)))
		h := imgui.NewVec2(float32(hit.Pos.X), -float32(hit.Pos.Y)).Mul(mmSizePx).Add(mazeOriginPx)

		drawList.AddCircleFilled(h, 4, imgui.ColorConvertFloat4ToU32(imgui.NewVec4(1, 0, 0, 1)))
		drawList.AddLineV(s, h, imgui.ColorConvertFloat4ToU32(imgui.NewVec4(1, 0, 0, 1)), 1.0)
	}

	imgui.End()
}

var dragging = false
var dragStartX, dragStartY float64
var dragButtonX, dragButtonY float32
