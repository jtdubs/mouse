#include "mode.h"

#include "adc.h"

mode_t mode = {0};

static const uint8_t FunctionSelectThrehsolds[16] = {21,  42,  60,  77,  91,  102, 112, 123,
                                                     133, 139, 144, 150, 156, 160, 163, 255};

void mode_update() {
  uint8_t v = (uint8_t)(adc_read(6) >> 2);

  mode.button = (v > 180);
  if (mode.button) {
    mode.active = mode.proposed;
    return;
  }

  for (int i = 0; i < 16; i++) {
    if (v < FunctionSelectThrehsolds[i]) {
      mode.proposed = 15 - i;
      return;
    }
  }
}
