#pragma once

// The contents of this file can be directly included into FX.cpp by defining USERFX1_CPP_H

#include <stdint.h>

uint16_t WS2812FX::mode_blink2(void) {
  return blink(SEGCOLOR(0), SEGCOLOR(1), false, true);
}
