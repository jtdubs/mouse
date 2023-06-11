package ui

import (
	"github.com/jtdubs/mouse/ui/pkg/mouse"

	imgui "github.com/AllenDang/cimgui-go"
)

type window interface {
	draw(mouse *mouse.Mouse)
}

type UI struct {
	backend imgui.Backend
	windows []window
}

func New() *UI {
	backend := imgui.CreateBackend(imgui.NewGLFWBackend())
	backend.SetBgColor(imgui.NewVec4(0.45, 0.55, 0.6, 1.0))
	backend.CreateWindow("Mouse UI", 1024, 768, 0)
	backend.SetTargetFPS(60)

	imgui.CurrentIO().Fonts().AddFontFromFileTTF("fonts/DroidSans.ttf", 24)
	imgui.CurrentIO().SetConfigFlags(imgui.ConfigFlagsDockingEnable)

	return &UI{
		backend: backend,
		windows: []window{
			newUSBWindow(),
		},
	}
}

func (ui *UI) Run(mouse *mouse.Mouse) {
	ui.backend.Run(func() {
		dockspace := imgui.DockSpaceOverViewport()
		for _, window := range ui.windows {
			imgui.SetNextWindowDockIDV(dockspace, imgui.CondFirstUseEver)
			window.draw(mouse)
		}
		ui.backend.Refresh()
	})
}
