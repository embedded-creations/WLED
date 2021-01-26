#pragma once

/*
 * Welcome! 
 * You can use the file "my_config.h" to make changes to the way WLED is compiled!
 * It is possible to enable and disable certain features as well as set defaults for some runtime changeable settings.
 * 
 * How to use:
 *   edit "my_config.h" directly, or copy the contents of one of the "my_config_sample*.h" files into "my_config.h" and edit as neeeded
 */

// force the compiler to show a warning to confirm that this file is included (comment this out to remove warning)
#warning **** my_config.h: Settings from this file are honored ****

//#define MAX_LEDS 1500       //Maximum total LEDs. More than 1500 might create a low memory situation on ESP8266.

//#define LEDPIN 27           // customize which pin to use to drive Neopixels

// Optionally use custom NpbWrapper with FastLED backend instead of NeoPixelBus - currently only Neopixels are supported
//#define CUSTOM_NPBWRAPPER_H "NpbWrapperFastLED.h"
