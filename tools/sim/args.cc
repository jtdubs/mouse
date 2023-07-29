#include "args.hh"

#include <cli11/CLI11.hh>

namespace app {

Args ParseArgs(int argc, char** argv) {
  CLI::App app{"Mouse Simulator"};
  Args     args = {.firmware_path = "", .enable_gdb = false};
  app.add_option("-f,--firmware", args.firmware_path, "Path to firmware")->required();
  app.add_flag("-g,--gdb,!--no-gdb", args.enable_gdb, "Enable GDB");
  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    app.exit(e);
  }
  return args;
}

}  // namespace app
