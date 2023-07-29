#include <iostream>

#include "args.hh"
#include "tools/remote/lib/remote/remote.hh"
#include "tools/remote/lib/ui/ui.hh"

int main(int argc, char** argv) {
  auto args = mouse::app::ParseArgs(argc, argv);

  auto remote = mouse::app::remote::Remote();
  if (!remote.Init(args.port)) {
    std::cerr << "Remote::Init failed" << std::endl;
    return 1;
  }

  auto ui = mouse::app::ui::New(&remote);
  ui->Run();

  return 0;
}
