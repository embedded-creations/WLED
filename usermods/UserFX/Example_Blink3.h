#pragma once

// This file can be included into WLED by defining USERFX#_H (e.g. USERFX1_H) with the path+filename to this file in quotes
// Also define USERFX#_NAME (e.g. USERFX1_NAME) with the name of your effect used in the definitions below
// e.g. if the name of your effect is BLINK2, the definitions below should be named "BLINK2_MODE_STRING" and "BLINK2_MODE_NAME"
// ENABLE_USERFX must be defined to enable the UserFX functionality

// This example effect's name is "BLINK3" - `#define USERFX1 BLINK3` will assign this BLINK3 effect to UserFX slot number 1

uint16_t mode_blink3(void) {
  return blink(SEGCOLOR(0), SEGCOLOR(1), false, true);
}

// set this to the name of your effect matching the name of the function defined above
#define BLINK3_MODE_NAME            mode_blink3

// set this to the name of your effect as you want it shown in the web interface
#define BLINK3_MODE_STRING          "blink 3"
