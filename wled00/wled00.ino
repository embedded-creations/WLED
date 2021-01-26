/*
 * WLED WS2812FX Effect Editor - Edit and Test WLED Effects with a Simplified Environment
 *
 * This sketch can run on the Wokwi.com AVR Simulator
 * - copy contents of "my_config_sample_wokwi.h" to "my_config.h" (creating file if needed)
 * - edit "wled.h", uncommenting definition of WLED_USE_MY_CONFIG
 * - edit stopLed0 below to match dimensions of simulation matrix: int stopLed0 = 16*16;
 * 
 * How to setup Wokwi
 * - Create an account and log in
 * - start with a relevant example:
 *   - FastLED/DigitalRain has a 16x16 matrix with square pixels: https://wokwi.com/arduino/libraries/FastLED/DigitalRain
 *     - Look for "pixelate": "1" in diagram.json which makes the square pixels (you can change to 0 or remove to make diffuse)
 *   - FastLED/XYMatrix has a 16x16 grid of neopixels: https://wokwi.com/arduino/libraries/FastLED/XYMatrix
 *   - FastLED/Cyon has an 8x8 diffused matrix: https://wokwi.com/arduino/libraries/FastLED/Cylon
 *   - FastLED/ColorPalette has a horizontal 50 led bar: https://wokwi.com/arduino/libraries/FastLED/ColorPalette
 *   - FastLED/Fire2012 has a vertical 30 LED bar: https://wokwi.com/arduino/libraries/FastLED/Fire2012
 *   - Careful, many examples use an Arduino Uno, not Mega, and we need the Mega for this sketch
 *     Edit "diagram.json" replacing "uno" with "mega" if needed
 * - Use the dropdown next to files to rename the sketch to the project name you want, I'm choosing "WLED_Test.ino"
 * - "Save", you should see the new filename, and the title of the browser window change to the new filename
 * - Refresh the browser, make sure you see the same names (check to make sure there's not a bug)
 * - "New File", give it a name, e.g. "const.h" as that's the first file alphabetically in this project
 * - Sdd some text to the file, doesn't matter what text
 * - "Save", then "Start the simulation" (it doesn't appear to save the file unless both are done)
 * - Refresh the browser
 * - If you don't see the const.h file (this is a bug I've seen myself):
 *   - "Save a copy"
 *   - Go to your project, find the original project that isn't saving properly and delete it
 *   - Open the copy, and do the above "const.h" test again until it's saving properly
 * - Edit "diagram.json" replacing "uno" with "mega" if needed
 * - Start copying files over from this project one by one (make sure you made the edits to "wled.h", "my_config.h", and this file)
 *   - Put the contents of this file in the .ino file (e.g. "WLED_Test.ino")
 *   - You only need to copy over "my_config.h" and not the samples
 *   - You only need to copy over "NpbWrapperFastLED.h" and not "NpbWrapper.h"
 * - Best workflow I've done to avoid data loss:
 *   - Add a new file, paste the contents, then every file or every few files,
       "Save", "Start the simulation" (get an error), then refresh the browser to make sure it saved and avoid annoying data loss
 * - Save
 * - "Start the simulation" with the play button, and it should compile and run
 *
 * Careful when editing this project in Wokwi, I've lost data several times.  I edit the project using a text editor,
 *   and copy and paste edited files into the Wokwi project to compile.
 * Specifically, I've lost data when trying to rename the .ino file.  It will save, but when you try to open the project
 *   again, the .ino file will have the previous name, and any "saved" changes were lost
 * Workaround to rename the .ino: Rename the .ino, "Save a copy", close the project and reopen and make sure it has
 *   the new name before continuing to edit the project
 * It seems you need to "Save", and "Start the simulation" to actually save the contents of the files
 *
 * Note that the 8-bit AVR's default data sizes aren't the same as the ESP8266/ESP32's,
 *   e.g. an "int" compiled on Wokwi is 16 bits and on Arduino IDE for ESP8266 is 32 bits
 *   and this may result in buggy behavior
 *
 * Sometimes the sketch seems to hang, usually a second or two after starting.  You'll see
 *   the timer in the upper right moving very slowly.  For now, just restart the sketch
 *   and after a try or two it should work
 * 
 * To customize this sketch for a different platform:
 * - Adjust options below
 * - Default LED setup is for driving Neopixels on pin 2.  If you need a different configuration:
 *   - edit "wled.h", uncommenting definition of WLED_USE_MY_CONFIG
 *   - Edit my_config.h - copying contents over from "my_config_sample.h" if needed
 *   - uncomment LEDPIN and set to the pin connected to your Neopixels,
 *     or see NpbWrapper.h for other options that can be set in my_config.h
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
int stopLed0 = 16*16;

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println("WLED Effects Testing");
  Serial.println("--------------------");

  strip.init(useRGBW, MAX_LEDS, skipFirstLed);
  strip.setBrightness(brightness);

  // do some of the things normally handled by WLED::Setup() and colorUpdated
  strip.setTransition(0);
  strip.trigger();

  // setup segment 0 with above options
  strip.setSegment(0, startLed0, stopLed0);

  // set mode manually here, sketch currently only runs one mode at a time
  strip.setMode(0, FX_MODE_SINELON_RAINBOW);
  //strip.setMode(0, FX_MODE_RIPPLE_RAINBOW);
  //strip.setMode(0, FX_MODE_BREATH);
  //strip.setMode(0, FX_MODE_BLINK);
}

void loop() {
  strip.service();
  delay(1);
}
