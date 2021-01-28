#pragma once

// this function takes the progressive matrix stored in leds[], applies mapping, and writes it to WLED's buffers
void writeLedsArrayToWled_XY(CRGB * leds) {
  const xyPair * xytable = _segmentmaps[_segment_index].xyTablePointer;

  // TODO: bounds check xytable using _segmentmaps[_segment_index].xyTableNumEntries, it's possible to set SEGLEN larger than table

  for (int i=0; i<SEGLEN; i++) {
     setPixelColor(i, leds[XY(xytable[i].x, xytable[i].y)].red, leds[XY(xytable[i].x, xytable[i].y)].green, leds[XY(xytable[i].x, xytable[i].y)].blue);
  }
}

// this function takes the contents of leds[], and writes it to WLED's buffers
void writeLedsArrayToWled(CRGB * leds) {
  for (int i=0; i<SEGLEN; i++) {
     setPixelColor(i, leds[i].red, leds[i].green, leds[i].blue);
  }
}
