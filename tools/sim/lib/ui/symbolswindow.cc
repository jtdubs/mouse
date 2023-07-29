#include <format>
#include <sstream>
#include <string>

#include "firmware/lib/control/linear.hh"
#include "firmware/lib/control/plan.hh"
#include "firmware/lib/control/rotational.hh"
#include "firmware/lib/control/walls.hh"
#include "firmware/lib/mode/explore/explore.hh"
#include "firmware/lib/utils/pid.hh"
#include "symbolswindow_impl.hh"
#include "textures_impl.hh"

namespace ui {

SymbolsWindow::SymbolsWindow(sim::Sim *sim) : Window(), sim_(sim), filter_(), hex_(false), names_() {}

void SymbolsWindow::Render() {
  if (!ImGui::Begin("Symbols")) {
    ImGui::End();
    return;
  }

  ImGui::Text("Filter:");
  ImGui::SameLine(0, 10);
  filter_.Draw("##SymbolFilter", 180);
  ImGui::SameLine();
  ImGui::Checkbox("Hex", &hex_);
  ImGui::Separator();

  auto flags = ImGuiTableFlags_Borders | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable;
  ImGui::BeginTable("##Symbols", 2, flags);
  ImGui::TableSetupColumn("Variable##SymbolVariables", ImGuiTableColumnFlags_WidthFixed, 280);
  ImGui::TableSetupColumn("Value##SymbolValues", ImGuiTableColumnFlags_WidthStretch);
  ImGui::TableHeadersRow();

  if (names_.size() == 0) {
    for (auto &[name, symbol] : sim_->GetSymbols()) {
      names_.push_back(name);
    }
    std::sort(names_.begin(), names_.end());
  }

  auto symbols = sim_->GetSymbols();
  for (auto name : names_) {
    if (!filter_.PassFilter(name.c_str())) {
      continue;
    }

    auto symbol = symbols.at(name);

    if (symbol.name == "explore::updates") {
      continue;
    }
    if (symbol.name == "maze::updates") {
      continue;
    }
    if (symbol.name == "report::report") {
      continue;
    }

    Row(symbol.name);
    if (symbol.name == "linear::state") {
      std::ostringstream os;
      os << reinterpret_cast<linear::State *>(symbol.data);
      ImGui::TextUnformatted(os.str().c_str());
    } else if (symbol.name == "rotational::state") {
      std::ostringstream os;
      os << reinterpret_cast<rotational::State *>(symbol.data);
      ImGui::TextUnformatted(os.str().c_str());
    } else if (symbol.name == "plan::current_plan") {
      std::ostringstream os;
      os << reinterpret_cast<plan::Plan *>(symbol.data);
      ImGui::TextUnformatted(os.str().c_str());
    } else if (symbol.name == "explore::next") {
      std::ostringstream os;
      os << reinterpret_cast<dequeue::Dequeue<maze::Location, 256> *>(symbol.data);
      ImGui::TextUnformatted(os.str().c_str());
    } else if (symbol.name == "explore::path") {
      std::ostringstream os;
      os << reinterpret_cast<dequeue::Dequeue<maze::Location, 256> *>(symbol.data);
      ImGui::TextUnformatted(os.str().c_str());
    } else if (symbol.name == "explore::orientation") {
      std::ostringstream os;
      os << *reinterpret_cast<explore::Orientation *>(symbol.data);
      ImGui::TextUnformatted(os.str().c_str());
    } else if (symbol.name == "walls::state") {
      std::ostringstream os;
      os << reinterpret_cast<walls::State *>(symbol.data);
      ImGui::TextUnformatted(os.str().c_str());
    } else if (symbol.name == "linear::wall_error_pid" ||  //
               symbol.name == "speed::pid_left" ||         //
               symbol.name == "speed::pid_right") {
      std::ostringstream os;
      os << reinterpret_cast<pid::PIController *>(symbol.data);
      ImGui::TextUnformatted(os.str().c_str());
    } else {
      switch (symbol.size) {
        case 1:
          if (hex_) {
            ImGui::TextUnformatted(std::format("0x{:02x}", *static_cast<uint8_t *>(symbol.data)).c_str());
          } else {
            ImGui::TextUnformatted(std::format("{:d}", *static_cast<uint8_t *>(symbol.data)).c_str());
          }
          break;
        case 2:
          if (hex_) {
            ImGui::TextUnformatted(std::format("0x{:04x}", *static_cast<uint16_t *>(symbol.data)).c_str());
          } else {
            ImGui::TextUnformatted(std::format("{:d}", *static_cast<uint16_t *>(symbol.data)).c_str());
          }
          break;
        case 4:
          if (symbol.name.find("theta") != std::string::npos) {
            ImGui::TextUnformatted(
                std::format("{:8.2f}", (*static_cast<float *>(symbol.data)) * 180.0f / std::numbers::pi).c_str());
          } else if (symbol.name.find("speed::") != std::string::npos) {
            ImGui::TextUnformatted(std::format("{:8.2f}", *static_cast<float *>(symbol.data)).c_str());
          } else if (symbol.name.find("position::") != std::string::npos) {
            ImGui::TextUnformatted(std::format("{:8.2f}", *static_cast<float *>(symbol.data)).c_str());
          } else if (symbol.name.find("linear::") != std::string::npos) {
            ImGui::TextUnformatted(std::format("{:8.2f}", *static_cast<float *>(symbol.data)).c_str());
          } else if (symbol.name.find("explore::cell_offset") != std::string::npos) {
            ImGui::TextUnformatted(std::format("{:8.2f}", *static_cast<float *>(symbol.data)).c_str());
          } else if (symbol.name.find("rotational::") != std::string::npos) {
            ImGui::TextUnformatted(std::format("{:8.2f}", *static_cast<float *>(symbol.data)).c_str());
          } else {
            if (hex_) {
              ImGui::TextUnformatted(std::format("0x{:08x}", *static_cast<uint32_t *>(symbol.data)).c_str());
            } else {
              ImGui::TextUnformatted(std::format("{:d}", *static_cast<uint32_t *>(symbol.data)).c_str());
            }
          }
          break;
        default: {
          auto data = static_cast<uint8_t *>(symbol.data);

          std::stringbuf buffer;
          std::ostream   os(&buffer);
          for (uint64_t i = 0; i < symbol.size; i++) {
            if (i % 8 == 0 && i != 0) {
              os << std::endl;
            }
            if (i % 8 == 0) {
              os << std::format("{:04x}:", i);
            }
            os << std::format(" {:02x}", data[i]);
          }
          os << std::endl;
          ImGui::TextUnformatted(buffer.str().c_str());
        } break;
      }
    }
  }

  ImGui::EndTable();
  ImGui::End();
}

}  // namespace ui
