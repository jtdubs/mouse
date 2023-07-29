#include <iostream>

#include "args.hh"
#include "tools/sim/lib/sim/sim.hh"
#include "tools/sim/lib/sim/symbols.hh"
#include "tools/sim/lib/ui/ui.hh"

int main(int argc, char** argv) {
  auto args = app::ParseArgs(argc, argv);

  auto sim = app::sim::Sim();
  if (!sim.Init(args.firmware_path, args.enable_gdb)) {
    std::cerr << "Sim::Init failed" << std::endl;
    return 1;
  }

  auto ui = app::ui::New(&sim);
  ui->Run();

  return 0;
}
