package ui

import (
	"context"
	"fmt"
	"path/filepath"
	"strings"

	"github.com/jtdubs/mouse/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

var Textures map[string]*imgui.Texture = make(map[string]*imgui.Texture)

type window interface {
	init()
	draw()
}

type UI struct {
	sim     *sim.Sim
	backend imgui.Backend
	windows []window
}

func New(sim *sim.Sim) *UI {
	backend := imgui.CreateBackend(imgui.NewGLFWBackend())

	ui := &UI{
		sim:     sim,
		backend: backend,
		windows: []window{
			newToolbarWindow(sim),
			newControlsWindow(sim),
			newSymbolsWindow(sim),
			newStatusWindow(sim),
		},
	}

	backend.SetBgColor(imgui.NewVec4(0.45, 0.55, 0.6, 1.0))
	backend.SetAfterCreateContextHook(ui.init)
	backend.CreateWindow("Mouse Simulator", 1600, 1200, imgui.GLFWWindowFlags(0))
	backend.SetTargetFPS(60)

	imgui.CurrentIO().Fonts().AddFontFromFileTTF("fonts/DroidSans.ttf", 24)
	imgui.CurrentIO().SetConfigFlags(imgui.ConfigFlagsDockingEnable)
	imgui.StyleColorsClassic()

	return ui
}

func (ui *UI) init() {
	files, _ := filepath.Glob("icons/*.png")
	for _, file := range files {
		name := strings.TrimSuffix(filepath.Base(file), filepath.Ext(file))
		Textures[name] = mustLoad(file)
	}

	for _, window := range ui.windows {
		window.init()
	}
}

func (ui *UI) Run(ctx context.Context) {
	firstRun := true

	go func() {
		<-ctx.Done()
		ui.backend.SetShouldClose(true)
	}()

	ui.backend.Run(func() {
		var dockspaceFlags imgui.WindowFlags = imgui.WindowFlagsNoDocking |
			imgui.WindowFlagsNoTitleBar |
			imgui.WindowFlagsNoCollapse |
			imgui.WindowFlagsNoResize |
			imgui.WindowFlagsNoMove |
			imgui.WindowFlagsNoBringToFrontOnFocus |
			imgui.WindowFlagsNoNavFocus

		vp := imgui.MainViewport()
		imgui.SetNextWindowPos(vp.Pos().Add(imgui.NewVec2(0, 48)))
		imgui.SetNextWindowSize(vp.Size().Sub(imgui.NewVec2(0, 48)))
		imgui.SetNextWindowViewport(vp.ID())
		imgui.PushStyleVarVec2(imgui.StyleVarWindowPadding, imgui.NewVec2(0, 0))
		imgui.PushStyleVarFloat(imgui.StyleVarWindowRounding, 0)
		imgui.PushStyleVarFloat(imgui.StyleVarWindowBorderSize, 0)
		imgui.BeginV("Dockspace", nil, dockspaceFlags)
		imgui.PopStyleVarV(3)
		dockID := imgui.IDStr("Dockspace")
		imgui.DockSpace(dockID)
		imgui.End()

		if firstRun {
			firstRun = false
			fmt.Println("Setting layout...")
			imgui.InternalDockBuilderRemoveNode(dockID)
			imgui.InternalDockBuilderAddNodeV(dockID, imgui.DockNodeFlagsDockSpace)
			imgui.InternalDockBuilderSetNodeSize(dockID, vp.Size().Sub(imgui.NewVec2(0, 48)))
			symbols := imgui.InternalDockBuilderSplitNode(dockID, imgui.DirRight, 0.3, nil, &dockID)
			status := imgui.InternalDockBuilderSplitNode(symbols, imgui.DirDown, 0.15, nil, &symbols)
			imgui.InternalDockBuilderDockWindow("Symbols", symbols)
			imgui.InternalDockBuilderDockWindow("Status", status)
			imgui.InternalDockBuilderDockWindow("Controls", dockID)
			imgui.InternalDockBuilderFinish(dockID)
		}

		for _, window := range ui.windows {
			window.draw()
		}

		ui.backend.Refresh()
	})
}

func mustLoad(image string) *imgui.Texture {
	img, err := imgui.LoadImage(image)
	if err != nil {
		panic(err)
	}
	tex := imgui.NewTextureFromRgba(img)
	fmt.Printf("Loaded %s (%vx%v) as %d\n", image, tex.Width, tex.Height, tex.ID())
	return tex
}
