#pragma once

#include <stdbool.h>
#include <stdint.h>

#pragma pack(push)
#pragma pack(1)
typedef struct {
  int  function : 4;
  int  padding  : 3;
  bool button   : 1;
} fsel_t;
#pragma pack(pop)

extern fsel_t fsel;

void fsel_init();
void fsel_update();
