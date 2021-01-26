/*
 */

#include "FX.h"

WS2812FX strip;

// describe your LEDs here
bool skipFirstLed = false;
bool useRGBW = false;         // useRGBW = true hasn't been tested
uint8_t brightness = 255;     // brightness is range from 0-255 
//uint8_t brightness = 20;    // Atom Matrix needs low brightness

// describe your segment 0 here
int startLed0 = 0;
int stopLed0 = 5*5;

void setup() {
  Serial.begin(115200);
  Serial.println("WLED Effects Testing");
  Serial.println("--------------------");

  strip.init(useRGBW, MAX_LEDS, skipFirstLed);
  strip.setBrightness(brightness);
  strip.setSegment(0, startLed0, stopLed0);

  // set mode manually here, sketch currently only runs one mode at a time
  strip.setMode(0, FX_MODE_RIPPLE_RAINBOW);
  //strip.setMode(0, FX_MODE_SINELON_RAINBOW);
}

void loop() {
  strip.service();
}
