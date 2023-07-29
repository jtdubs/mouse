#include "args.hh"

#include <CLI/CLI.hpp>

namespace mouse::app {

Args ParseArgs(int argc, char** argv) {
  CLI::App app{"Mouse Remote"};
  Args     args = {.port = ""};
  app.add_option("-p,--port", args.port, "Serial port")->required();
  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    app.exit(e);
  }
  return args;
}

}  // namespace mouse::app
