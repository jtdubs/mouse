#pragma once

#include <string>

namespace app {

struct Args {
  std::string port;
};

Args ParseArgs(int argc, char **argv);

}  // namespace app
