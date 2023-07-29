#pragma once

#include <string>

namespace mouse::app {

struct Args {
  std::string firmware_path;
  bool        enable_gdb;
};

Args ParseArgs(int argc, char **argv);

}  // namespace mouse::app
