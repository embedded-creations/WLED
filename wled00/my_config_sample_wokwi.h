#pragma once

/*
 * Welcome! 
 * You can use the file "my_config.h" to make changes to the way WLED is compiled!
 * It is possible to enable and disable certain features as well as set defaults for some runtime changeable settings.
 * 
 * How to use:
 *   edit "my_config.h" directly, or copy the contents of one of the "my_config_sample*.h" files into "my_config.h" and edit as neeeded
 */

// the simulator is compatible with FastLED, so use the FastLED wrapper
#define CUSTOM_NPBWRAPPER_H "NpbWrapperFastLED.h"

#define MAX_LEDS (512 + 1) // There doesn't seem to be enough memory (or it takes too long to send out 1500 neopixels) with the Wokwi simulator

#define LEDPIN 3      // This pin is used by the simulator

#define WLED_DISABLE_FX_HIGH_FLASH_USE
