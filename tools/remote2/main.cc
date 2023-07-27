#include "args.hh"
#include "tools/remote2/lib/remote/remote.hh"
#include "tools/remote2/lib/ui/ui.hh"

int main(int argc, char** argv) {
  auto args = ParseArgs(argc, argv);

  auto remote = remote::Remote();
  if (!remote.Init(args.port)) {
    fprintf(stderr, "Remote::Init failed\n");
    return 1;
  }

  auto ui = ui::UI(&remote);
  ui.Run();

  return 0;
}
