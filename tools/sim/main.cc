#include "args.hh"
#include "tools/sim/lib/sim/sim.hh"
#include "tools/sim/lib/sim/symbols.hh"
#include "tools/sim/lib/ui/ui.hh"

int main(int argc, char** argv) {
  auto args = ParseArgs(argc, argv);

  auto sim = sim::Sim();
  if (!sim.Init(args.firmware_path, args.enable_gdb)) {
    fprintf(stderr, "Sim::Load failed\n");
    return 1;
  }

  auto ui = ui::UI(&sim);
  ui.Run();

  return 0;
}
