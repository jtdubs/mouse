#pragma once

#include <stdbool.h>

typedef struct {
  char *firmware_path;
  bool  gdb_enabled;
} arguments_t;

arguments_t *parse_args();
