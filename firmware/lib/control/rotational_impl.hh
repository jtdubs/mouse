#pragma once

#include <stdbool.h>

#include "rotational.hh"

namespace mouse::control::rotational {

// Init initializes the rotational motion module.
void Init();

// start starts a rotational motion.
void Start(float dtheta /* radians */);

// Tick updates the platform to perform the rotation.
bool Tick();

// state reads the current rotational state.
void Read(State &state);

}  // namespace mouse::control::rotational
