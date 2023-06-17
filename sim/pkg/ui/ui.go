package ui

import (
	"context"

	"github.com/jtdubs/mouse/sim/pkg/sim"

	imgui "github.com/AllenDang/cimgui-go"
)

type window interface {
	draw(sim *sim.Sim)
}

type UI struct {
	sim     *sim.Sim
	backend imgui.Backend
	windows []window
}

func New(sim *sim.Sim) *UI {
	backend := imgui.CreateBackend(imgui.NewGLFWBackend())
	backend.SetBgColor(imgui.NewVec4(0.45, 0.55, 0.6, 1.0))
	backend.CreateWindow("Mouse Simulator", 1024, 768, imgui.GLFWWindowFlags(0))
	backend.SetTargetFPS(60)

	imgui.CurrentIO().Fonts().AddFontFromFileTTF("fonts/DroidSans.ttf", 24)
	imgui.CurrentIO().SetConfigFlags(imgui.ConfigFlagsDockingEnable)

	imgui.StyleColorsClassic()

	return &UI{
		sim:     sim,
		backend: backend,
		windows: []window{
			newSimWindow(),
		},
	}
}

func (ui *UI) Run(ctx context.Context) {
	go func() {
		<-ctx.Done()
		ui.backend.SetShouldClose(true)
	}()
	ui.backend.Run(func() {
		dockspace := imgui.DockSpaceOverViewport()
		for _, window := range ui.windows {
			imgui.SetNextWindowDockIDV(dockspace, imgui.CondFirstUseEver)
			window.draw(ui.sim)
		}
		ui.backend.Refresh()
	})
}
