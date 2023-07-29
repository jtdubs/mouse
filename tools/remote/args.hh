#pragma once

#include <string>

namespace mouse::app {

struct Args {
  std::string port;
};

Args ParseArgs(int argc, char **argv);

}  // namespace mouse::app
