#include "fsel.h"

#include "adc.h"

fsel_t fsel;

static const uint8_t FunctionSelectThrehsolds[16] = {21,  42,  60,  77,  91,  102, 112, 123,
                                                     133, 139, 144, 150, 156, 160, 163, 255};

void fsel_init() {
  fsel = (fsel_t){
      .button   = false,
      .function = 0,
  };
}

void fsel_update() {
  uint8_t v   = (uint8_t)(adc_read(6) >> 2);
  fsel.button = (v > 180);
  if (fsel.button) {
    return;
  }
  for (int i = 0; i < 16; i++) {
    if (v < FunctionSelectThrehsolds[i]) {
      fsel.function = 15 - i;
      return;
    }
  }
}
