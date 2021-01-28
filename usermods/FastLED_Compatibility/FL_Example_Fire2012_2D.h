#pragma once

// This example effect's name is "FIRE2012_2D" - `#define USERFX1 FIRE2012_2D` will assign this FIRE2012_2D effect to UserFX slot number 1

// Fire2012 by Mark Kriegsman. Converted to WLED by Andrew Tuline. 
// original https://github.com/FastLED/FastLED/blob/master/examples/Fire2012/Fire2012.ino
uint16_t mode_2Dfire2012(void) {
  FL_ALLOC_WITH_1_ARRAY_XY(
    unsigned long prevMillis;
    , byte, heat, NUM_LEDS
    );

  const uint8_t COOLING = 50;
  const uint8_t SPARKING = 50;

  CRGBPalette16 currentPalette  = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Orange, CRGB::Yellow);
  unsigned long curMillis = millis();

  if ((curMillis - FL_STATICVAR(prevMillis)) >= ((256-SEGMENT.speed) >>2)) {
    FL_STATICVAR(prevMillis) = curMillis;

    for (int mw = 0; mw < kMatrixWidth; mw++) {            // Move along the width of the flame

      // Step 1.  Cool down every cell a little
      for (int mh = 0; mh < kMatrixHeight; mh++) {
        FL_STATICVAR(heat)[mw*kMatrixWidth+mh] = qsub8( FL_STATICVAR(heat)[mw*kMatrixWidth+mh],  random16(0, ((COOLING * 10) / kMatrixHeight) + 2));
      }

      // Step 2.  Heat from each cell drifts 'up' and diffuses a little
      for (int mh = kMatrixHeight - 1; mh >= 2; mh--) {
        FL_STATICVAR(heat)[mw*kMatrixWidth+mh] = (FL_STATICVAR(heat)[mw*kMatrixWidth+mh - 1] + FL_STATICVAR(heat)[mw*kMatrixWidth+mh - 2] + FL_STATICVAR(heat)[mw*kMatrixWidth+mh - 2] ) / 3;
      }

      // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
      if (random8(0,255) < SPARKING ) {
        int mh = random8(3);
        FL_STATICVAR(heat)[mw*kMatrixWidth+mh] = qadd8( FL_STATICVAR(heat)[mw*kMatrixWidth+mh], random8(160,255) );
      }

      // Step 4.  Map from heat cells to LED colors
      for (int mh = 0; mh < kMatrixHeight; mh++) {
        byte colorindex = scale8( FL_STATICVAR(heat)[mw*kMatrixWidth+mh], 240);
        leds[XY(mw,mh)] = ColorFromPalette(currentPalette, colorindex, 255);
      } // for mh
    } // for mw
  } // if millis
  // TODO: we might not need to update leds every time based on millis check?

  END_FASTLED_XY_COMPATIBILITY();

  return FRAMETIME;
}

// set this to the name of your effect matching the name of the function defined above
#define FIRE2012_2D_MODE_NAME            mode_2Dfire2012

// set this to the name of your effect as you want it shown in the web interface
#define FIRE2012_2D_MODE_STRING          "*2D Fire"
