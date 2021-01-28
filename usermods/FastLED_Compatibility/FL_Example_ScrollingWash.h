#pragma once

// This example effect's name is "SCROLLING_WASH" - `#define USERFX1 SCROLLING_WASH` will assign this SCROLLING_WASH effect to UserFX slot number 1

uint16_t mode_ScrollingWash(void) {
  BEGIN_FASTLED_COMPATIBILITY();

  const xyPair * xytable = _segmentmaps[_segment_index].xyTablePointer;

  for (int i=0; i<NUM_LEDS; i++) {
    uint16_t x = xytable[i].x;
    uint16_t y = xytable[i].y;
    leds[i] = CHSV( x + (millis() / 10), 255, 255);
  }

  END_FASTLED_COMPATIBILITY();

  return FRAMETIME;
}

// set this to the name of your effect matching the name of the function defined above
#define SCROLLING_WASH_MODE_NAME            mode_ScrollingWash

// set this to the name of your effect as you want it shown in the web interface
#define SCROLLING_WASH_MODE_STRING          "* 2D Scrolling Wash"
