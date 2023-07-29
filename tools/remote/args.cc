#include "args.hh"

#include <cli11/CLI11.hh>

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
