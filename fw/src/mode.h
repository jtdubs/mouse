#pragma once

#include <stdbool.h>
#include <stdint.h>

#define MODE_REMOTE 0
#define MODE_WALL 1

#pragma pack(push)
#pragma pack(1)
typedef struct {
  uint8_t active   : 3;
  uint8_t proposed : 3;
  int     padding  : 1;
  bool    button   : 1;
} mode_t;
#pragma pack(pop)

extern mode_t mode;

void mode_update();
