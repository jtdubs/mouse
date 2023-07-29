#include <format>

#include "imgui_internal.h"
#include "toolbarwindow_impl.hh"

namespace app::ui {

ToolbarWindow::ToolbarWindow(app::sim::Sim *sim) : ::ui::ToolbarWindow(), sim_(sim) {}

void ToolbarWindow::RenderToolbar() {
  if (sim_->GetState() == sim::State::Crashed) {
    ImGui::Text("Crashed");
    return;
  }
  if (sim_->GetState() == sim::State::Done) {
    ImGui::Text("Done");
    return;
  }

  if (sim_->GetState() == sim::State::Running) {
    if (IconButton("SimPause", "pause-black")) {
      sim_->Pause();
    }
  } else {
    if (IconButton("SimPlay", "play-black")) {
      sim_->Run();
    }
  }
  ImGui::SameLine();
  ImGui::BeginDisabled(sim_->GetState() == sim::State::Running);
  if (IconButton("SimStep10ms", "step-forward-black")) {
    sim_->RunNearest(10 * 1000 * 1000);
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("10ms");
  }
  ImGui::SameLine();
  if (IconButton("SimStep1s", "step-forward-2-black")) {
    sim_->RunNearest(1000 * 1000 * 1000);
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("1s");
  }
  ImGui::SameLine();
  if (IconButton("SimSkip", "skip-forward")) {
    // TODO: skip to next plan change
  }
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("Next plan change");
  }
  ImGui::EndDisabled();

  ImGui::SameLine(0, 20);

  ImGui::BeginDisabled(sim_->IsRecording());
  if (IconButton("SimRecord", "video-outline-black")) {
    sim_->SetRecording(true);
  }
  ImGui::EndDisabled();
  ImGui::SameLine();
  ImGui::BeginDisabled(!sim_->IsRecording());
  if (IconButton("SimRecord", "video-off-outline-black")) {
    sim_->SetRecording(false);
  }
  ImGui::EndDisabled();

  ImGui::SameLine(0, 20);

  auto nanos      = sim_->GetNanos();
  auto nanos_text = std::format("{}:{:02d}:{:02d}:{:02d}.{:03d},{:03d},{:03d}",  //
                                (nanos / 3600000000000),                         //
                                (nanos / 60000000000) % 60,                      //
                                (nanos / 1000000000) % 60,                       //
                                (nanos / 1000000) % 1000,                        //
                                (nanos / 1000) % 1000,                           //
                                nanos % 1000,                                    //
                                nanos);

  ImGui::SetCursorPosY((48 - ImGui::CalcTextSize(nanos_text.c_str()).y) / 2);
  ImGui::TextUnformatted(nanos_text.c_str());
}

}  // namespace app::ui
