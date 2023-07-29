#include <iostream>

#include "args.hh"
#include "tools/sim/lib/sim/sim.hh"
#include "tools/sim/lib/sim/symbols.hh"
#include "tools/sim/lib/ui/ui.hh"

int main(int argc, char** argv) {
  auto args = ParseArgs(argc, argv);

  auto sim = sim::Sim();
  if (!sim.Init(args.firmware_path, args.enable_gdb)) {
    std::cerr << "Sim::Init failed" << std::endl;
    return 1;
  }

  auto ui = ui::UI(&sim);
  ui.Run();

  return 0;
}
