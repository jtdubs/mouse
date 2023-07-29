#pragma once

#include "command.hh"

namespace mouse::mode::remote {

// Init initializes the command module.
void Init();

// processed indicates the command has been processed.
void Processed();

// next gets the next command, if one is available.
bool Next(Command &command);

}  // namespace mouse::mode::remote
