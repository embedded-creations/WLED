#pragma once

// The contents of this file can be directly included into FX.cpp by defining USERFX2_CPP_H

#include <stdint.h>
#include "sound_reactive.h"

uint16_t WS2812FX::mode_blink3(void) {
  return blink(SEGCOLOR(0), SEGCOLOR(1), false, true);
}

/////////////////////////////////////////
//    Start of Audio Reactive fork     //
/////////////////////////////////////////

// FastLED array, so we can refer to leds[i] instead of the lossy getPixel() and setPixel()
uint32_t ledData[MAX_LEDS];                     // See const.h for a value of 1500.
uint32_t dataStore[4096];                       // we are declaring a storage area or 64 x 64 (4096) words.

extern byte soundSquelch;

///////////////////////////////////////
// Helper function(s)                //
///////////////////////////////////////

////////////////////////////
//       set Pixels       //
////////////////////////////

void WS2812FX::setPixels(CRGB* leds) {
   for (int i=0; i<SEGLEN; i++) {
      setPixelColor(i, leds[i].red, leds[i].green, leds[i].blue);
   }
}



/////////////////////////////
//  Non-Reactive Routines  //
/////////////////////////////

/////////////////////////
//     Perlin Move     //
/////////////////////////

// 16 bit perlinmove. Use Perlin Noise instead of sinewaves for movement. By Andrew Tuline.
// Controls are speed, # of pixels, faderate.

uint16_t WS2812FX::mode_perlinmove(void) {

  fade_out(255-SEGMENT.fft1);
  for (int i=0; i<SEGMENT.intensity/16+1; i++) {
    uint16_t locn = inoise16(millis()*128/(260-SEGMENT.speed)+i*15000, millis()*128/(260-SEGMENT.speed));   // Get a new pixel location from moving noise.
    uint16_t pixloc = map(locn,50*256,192*256,0,SEGLEN)%(SEGLEN);                       // Map that to the length of the strand, and ensure we don't go over.
    setPixelColor(pixloc, color_blend(SEGCOLOR(1), color_from_palette(pixloc%255, false, PALETTE_SOLID_WRAP, 0), 255));
  }

  return FRAMETIME;
} // mode_perlinmove()


////////////////////////////////
//   Begin volume routines    //
////////////////////////////////

//////////////////////
//     * PIXELS     //
//////////////////////

uint16_t WS2812FX::mode_pixels(void) {                    // Pixels. By Andrew Tuline.

  fade_out(4);

  for (int i=0; i <SEGMENT.intensity/16; i++) {
    uint16_t segLoc = random(SEGLEN);                     // 16 bit for larger strands of LED's.
    setPixelColor(segLoc, color_blend(SEGCOLOR(1), color_from_palette(myVals[i%32]+i*4, false, PALETTE_SOLID_WRAP, 0), sampleAgc));
  }

  return FRAMETIME;
} // mode_pixels()


//////////////////////
//   * PIXELWAVE    //
//////////////////////

uint16_t WS2812FX::mode_pixelwave(void) {                 // Pixelwave. By Andrew Tuline.

  CRGB *leds = (CRGB*) ledData;
  if (SEGENV.call == 0) fill_solid(leds,SEGLEN, 0);
  uint8_t secondHand = micros()/(256-SEGMENT.speed)/500+1 % 16;

  if(SEGENV.aux0 != secondHand) {
    SEGENV.aux0 = secondHand;
    int pixBri = sample * SEGMENT.intensity / 64;
    leds[SEGLEN/2] = color_blend(SEGCOLOR(1), color_from_palette(millis(), false, PALETTE_SOLID_WRAP, 0), pixBri);

    for (int i=SEGLEN-1; i>SEGLEN/2; i--) {               // Move to the right.
      leds[i] = leds[i-1];
    }
    for (int i=0; i<SEGLEN/2; i++) {                      // Move to the left.
      leds[i]=leds[i+1];
    }
  }

  setPixels(leds);
  return FRAMETIME;
} // mode_pixelwave()

//////////////////////
//   * JUGGLES      //
//////////////////////

uint16_t WS2812FX::mode_juggles(void) {                   // Juggles. By Andrew Tuline.

  fade_out(224);
  for (int i=0; i<SEGMENT.intensity/32+1; i++) {
          setPixelColor(beatsin16(SEGMENT.speed/4+i*2,0,SEGLEN-1), color_blend(SEGCOLOR(1), color_from_palette(millis()/4+i*2, false, PALETTE_SOLID_WRAP, 0), sampleAgc));
  }

  return FRAMETIME;
} // mode_juggles()


//////////////////////
//   * MATRIPIX     //
//////////////////////

uint16_t WS2812FX::mode_matripix(void) {                  // Matripix. By Andrew Tuline.
  CRGB *leds = (CRGB*) ledData;
  if (SEGENV.call == 0) fill_solid(leds,SEGLEN, 0);

  uint8_t secondHand = micros()/(256-SEGMENT.speed)/500 % 16;
  if(SEGENV.aux0 != secondHand) {
    SEGENV.aux0 = secondHand;
    int pixBri = sample * SEGMENT.intensity / 64;
    leds[SEGLEN-1] = color_blend(SEGCOLOR(1), color_from_palette(millis(), false, PALETTE_SOLID_WRAP, 0), pixBri);
    for (int i=0; i<SEGLEN-1; i++) leds[i] = leds[i+1];
  }

  setPixels(leds);
  return FRAMETIME;
} // mode_matripix()


///////////////////////
//   * GRAVIMETER    //
///////////////////////

uint16_t WS2812FX::mode_gravimeter(void) {                // Gravmeter. By Andrew Tuline.

  static int topLED;
  static int gravityCounter = 0;

  fade_out(240);

  sampleAvg = sampleAvg * SEGMENT.intensity / 255;

  int tempsamp = constrain(sampleAvg*2,0,SEGLEN-1);       // Keep the sample from overflowing.
  uint8_t gravity = 8 - SEGMENT.speed/32;

  for (int i=0; i<tempsamp; i++) {
    uint8_t index = inoise8(i*sampleAvg+millis(), 5000+i*sampleAvg);
    setPixelColor(i, color_blend(SEGCOLOR(1), color_from_palette(index, false, PALETTE_SOLID_WRAP, 0), sampleAvg*8));
  }

  if (tempsamp >= topLED)
    topLED = tempsamp;
  else if (gravityCounter % gravity == 0)
    topLED--;

  if (topLED > 0) {
    setPixelColor(topLED, color_blend(SEGCOLOR(1), color_from_palette(millis(), false, PALETTE_SOLID_WRAP, 0), 255));
  }
  gravityCounter = (gravityCounter + 1) % gravity;

  return FRAMETIME;
} // mode_gravimeter()


///////////////////////
//   * GRAVCENTER    //
///////////////////////

uint16_t WS2812FX::mode_gravcenter(void) {                // Gravcenter. By Andrew Tuline.

  static int topLED;
  static int gravityCounter = 0;

  fade_out(240);

  sampleAvg = sampleAvg * SEGMENT.intensity / 255;

  int tempsamp = constrain(sampleAvg*2,0,SEGLEN/2-1);     // Keep the sample from overflowing.
  uint8_t gravity = 8 - SEGMENT.speed/32;

  for (int i=0; i<tempsamp; i++) {
    uint8_t index = inoise8(i*sampleAvg+millis(), 5000+i*sampleAvg);
    setPixelColor(i+SEGLEN/2, color_blend(SEGCOLOR(1), color_from_palette(index, false, PALETTE_SOLID_WRAP, 0), sampleAvg*8));
    setPixelColor(SEGLEN/2-i, color_blend(SEGCOLOR(1), color_from_palette(index, false, PALETTE_SOLID_WRAP, 0), sampleAvg*8));
  }

  if (tempsamp >= topLED)
    topLED = tempsamp;
  else if (gravityCounter % gravity == 0)
    topLED--;

  if (topLED > 0) {
    setPixelColor(topLED+SEGLEN/2, color_blend(SEGCOLOR(1), color_from_palette(millis(), false, PALETTE_SOLID_WRAP, 0), 255));
    setPixelColor(SEGLEN/2-topLED, color_blend(SEGCOLOR(1), color_from_palette(millis(), false, PALETTE_SOLID_WRAP, 0), 255));
  }
  gravityCounter = (gravityCounter + 1) % gravity;

  return FRAMETIME;
} // mode_gravcenter()



///////////////////////
//   * GRAVCENTRIC   //
///////////////////////

uint16_t WS2812FX::mode_gravcentric(void) {               // Gravcenter. By Andrew Tuline.

  static int topLED;
  static int gravityCounter = 0;

  fade_out(240);
  fade_out(240);

  sampleAvg = sampleAvg * SEGMENT.intensity / 255;

  int tempsamp = constrain(sampleAvg*2,0,SEGLEN/2-1);     // Keep the sample from overflowing.
  uint8_t gravity = 8 - SEGMENT.speed/32;

  for (int i=0; i<tempsamp; i++) {
    uint8_t index = sampleAvg*24+millis()/200;
    setPixelColor(i+SEGLEN/2, color_blend(SEGCOLOR(0), color_from_palette(index, false, PALETTE_SOLID_WRAP, 0), 255));
    setPixelColor(SEGLEN/2-i, color_blend(SEGCOLOR(0), color_from_palette(index, false, PALETTE_SOLID_WRAP, 0), 255));
  }

  if (tempsamp >= topLED)
    topLED = tempsamp;
  else if (gravityCounter % gravity == 0)
    topLED--;

  if (topLED > 0) {
    setPixelColor(topLED+SEGLEN/2, CRGB::Gray);
    setPixelColor(SEGLEN/2-topLED, CRGB::Gray);
  }
  gravityCounter = (gravityCounter + 1) % gravity;

  return FRAMETIME;
} // mode_gravcentric()


//////////////////////
//   * MIDNOISE     //
//////////////////////

uint16_t WS2812FX::mode_midnoise(void) {                  // Midnoise. By Andrew Tuline.

  static uint16_t xdist;
  static uint16_t ydist;

  fade_out(SEGMENT.speed);

  uint16_t maxLen = sampleAvg * SEGMENT.intensity / 256;  // Too sensitive.
  maxLen = maxLen * SEGMENT.intensity / 128;              // Reduce sensitity/length.

  if (maxLen >SEGLEN/2) maxLen = SEGLEN/2;

  for (int i=(SEGLEN/2-maxLen); i<(SEGLEN/2+maxLen); i++) {
    uint8_t index = inoise8(i*sampleAvg+xdist, ydist+i*sampleAvg);  // Get a value from the noise function. I'm using both x and y axis.
    setPixelColor(i, color_blend(SEGCOLOR(1), color_from_palette(index, false, PALETTE_SOLID_WRAP, 0), 255));
  }

  xdist=xdist+beatsin8(5,0,10);
  ydist=ydist+beatsin8(4,0,10);

  return FRAMETIME;
} // mode_midnoise()


//////////////////////
//   * NOISEFIRE    //
//////////////////////

// I am the god of hellfire. . . Volume (only) reactive fire routine. Oh, look how short this is.
uint16_t WS2812FX::mode_noisefire(void) {                 // Noisefire. By Andrew Tuline.

  CRGB *leds = (CRGB*) ledData;

  currentPalette = CRGBPalette16(CHSV(0,255,2), CHSV(0,255,4), CHSV(0,255,8), CHSV(0, 255, 8),  // Fire palette definition. Lower value = darker.
                                 CHSV(0, 255, 16), CRGB::Red, CRGB::Red, CRGB::Red,
                                 CRGB::DarkOrange,CRGB::DarkOrange, CRGB::Orange, CRGB::Orange,
                                 CRGB::Yellow, CRGB::Orange, CRGB::Yellow, CRGB::Yellow);

  for (int i = 0; i < SEGLEN; i++) {
    uint16_t index = inoise8(i*SEGMENT.speed/64,millis()*SEGMENT.speed/64*SEGLEN/255);  // X location is constant, but we move along the Y at the rate of millis(). By Andrew Tuline.
    index = (255 - i*256/SEGLEN) * index/(256-SEGMENT.intensity);                       // Now we need to scale index so that it gets blacker as we get close to one of the ends.
                                                                                        // This is a simple y=mx+b equation that's been scaled. index/128 is another scaling.
    CRGB color = ColorFromPalette(currentPalette, index, sampleAvg*2, LINEARBLEND);     // Use the my own palette.
    leds[i] = color;
  }

  setPixels(leds);
  return FRAMETIME;
} // mode_noisefire()


///////////////////////
//   * Noisemeter    //
///////////////////////

uint16_t WS2812FX::mode_noisemeter(void) {                // Noisemeter. By Andrew Tuline.

  static uint16_t xdist;
  static uint16_t ydist;

  uint8_t fadeRate = map(SEGMENT.speed,0,255,224,255);
  fade_out(fadeRate);

  int maxLen = sample*8;

  maxLen = maxLen * SEGMENT.intensity / 256;              // Still a bit too sensitive.

  if (maxLen >SEGLEN) maxLen = SEGLEN;

  for (int i=0; i<maxLen; i++) {                                    // The louder the sound, the wider the soundbar. By Andrew Tuline.
    uint8_t index = inoise8(i*sampleAvg+xdist, ydist+i*sampleAvg);  // Get a value from the noise function. I'm using both x and y axis.
    setPixelColor(i, color_blend(SEGCOLOR(1), color_from_palette(index, false, PALETTE_SOLID_WRAP, 0), 255));
  }

  xdist+=beatsin8(5,0,10);
  ydist+=beatsin8(4,0,10);

  return FRAMETIME;
} // mode_noisemeter()


//////////////////////
//   * PLASMOID     //
//////////////////////

uint16_t WS2812FX::mode_plasmoid(void) {                  // Plasmoid. By Andrew Tuline.

  fade_out(224);

  static int16_t thisphase = 0;                           // Phase of a cubicwave8.
  static int16_t thatphase = 0;                           // Phase of the cos8.

  uint8_t thisbright;
  uint8_t colorIndex;

  thisphase += beatsin8(6,-4,4);                          // You can change direction and speed individually.
  thatphase += beatsin8(7,-4,4);                          // Two phase values to make a complex pattern. By Andrew Tuline.

  for (int i=0; i<SEGLEN; i++) {                          // For each of the LED's in the strand, set a brightness based on a wave as follows.
    thisbright = cubicwave8((i*13)+thisphase)/2;
    thisbright += cos8((i*117)+thatphase)/2;              // Let's munge the brightness a bit and animate it all with the phases.
    colorIndex=thisbright;

    if (sampleAvg * 8 * SEGMENT.intensity/256 > thisbright) {thisbright = 255;} else {thisbright = 0;}
    setPixelColor(i, color_blend(SEGCOLOR(1), color_from_palette(colorIndex, false, PALETTE_SOLID_WRAP, 0), thisbright));
  }

  return FRAMETIME;
} // mode_plasmoid()


//////////////////////
//   * PUDDLES      //
//////////////////////

uint16_t WS2812FX::mode_puddles(void) {                   // Puddles. By Andrew Tuline.

  uint16_t size = 0;
  uint8_t fadeVal = map(SEGMENT.speed,0,255, 224, 255);
  uint16_t pos = random(SEGLEN);                          // Set a random starting position.

  fade_out(fadeVal);

  if (sample>0 ) {
    size = sample * SEGMENT.intensity /256 /8 + 1;        // Determine size of the flash based on the volume.
    if (pos+size>= SEGLEN) size=SEGLEN-pos;
  }

  for(int i=0; i<size; i++) {                             // Flash the LED's.
    setPixelColor(pos+i, color_blend(SEGCOLOR(1), color_from_palette(millis(), false, PALETTE_SOLID_WRAP, 0), 255));
  }

  return FRAMETIME;
} // mode_puddles()


///////////////////////
//   * PUDDLEPEAK    //
///////////////////////

// Andrew's crappy peak detector. If I were 40+ years younger, I'd learn signal processing.
uint16_t WS2812FX::mode_puddlepeak(void) {                // Puddlepeak. By Andrew Tuline.

  uint16_t size = 0;
  uint8_t fadeVal = map(SEGMENT.speed,0,255, 224, 255);
  uint16_t pos = random(SEGLEN);                          // Set a random starting position.

  fade_out(fadeVal);

  if (samplePeak == 1 ) {
    size = sampleAgc * SEGMENT.intensity /256 /4 + 1;     // Determine size of the flash based on the volume.
    if (pos+size>= SEGLEN) size=SEGLEN-pos;
  }

  for(int i=0; i<size; i++) {                             // Flash the LED's.
    setPixelColor(pos+i, color_blend(SEGCOLOR(1), color_from_palette(millis(), false, PALETTE_SOLID_WRAP, 0), 255));

  }

  return FRAMETIME;
} // mode_puddlepeak()


/////////////////////////////////
//     * Ripple Peak           //
/////////////////////////////////

uint16_t WS2812FX::mode_ripplepeak(void) {                // * Ripple peak. By Andrew Tuline.

                                                          // This currently has no controls.
  #define maxsteps 16                                     // Case statement wouldn't allow a variable.

  uint16_t maxRipples = 16;
  uint16_t dataSize = sizeof(ripple) * maxRipples;

  if (!SEGENV.allocateData(dataSize)) return mode_static(); //allocation failed

  Ripple* ripples = reinterpret_cast<Ripple*>(SEGENV.data);

//  static uint8_t colour;                                  // Ripple colour is randomized.
//  static uint16_t centre;                                 // Center of the current ripple.
//  static int8_t steps = -1;                               // -1 is the initializing step.
  static uint8_t ripFade = 255;                           // Starting brightness.

  fade_out(240);                                          // Lower frame rate means less effective fading than FastLED
  fade_out(240);


  for (uint16_t i = 0; i < SEGMENT.intensity/16; i++) {   // Limit the number of ripples.

    if (samplePeak) {

      ripples[i].state = -1;
    }

    switch (ripples[i].state) {

       case -2:     // Inactive mode
        break;

       case -1:                                           // Initialize ripple variables.
        ripples[i].pos = random16(SEGLEN);

        #ifdef ESP32
          ripples[i].color = (int)(log10(FFT_MajorPeak)*128);
        #else
          ripples[i].color = random8();
        #endif

        ripples[i].state = 0;
        break;

      case 0:
        setPixelColor(ripples[i].pos, color_blend(SEGCOLOR(1), color_from_palette(ripples[i].color, false, PALETTE_SOLID_WRAP, 0), ripFade));
        ripples[i].state++;
        break;

      case maxsteps:                                      // At the end of the ripples. -2 is an inactive mode.
          ripples[i].state = -2;
        break;

      default:                                            // Middle of the ripples.

        setPixelColor((ripples[i].pos + ripples[i].state + SEGLEN) % SEGLEN, color_blend(SEGCOLOR(1), color_from_palette(ripples[i].color, false, PALETTE_SOLID_WRAP, 0), ripFade/ripples[i].state*2));
        setPixelColor((ripples[i].pos - ripples[i].state + SEGLEN) % SEGLEN, color_blend(SEGCOLOR(1), color_from_palette(ripples[i].color, false, PALETTE_SOLID_WRAP, 0), ripFade/ripples[i].state*2));
        ripples[i].state++;                               // Next step.
        break;
    } // switch step
   } // for i


  return FRAMETIME;
} // mode_ripplepeak()



///////////////////////////////
//     BEGIN FFT ROUTINES    //
///////////////////////////////

double mapf(double x, double in_min, double in_max, double out_min, double out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

////////////////////
//    **Binmap    //
////////////////////

uint16_t WS2812FX::mode_binmap(void) {                    // Binmap. Scale raw fftBin[] values to SEGLEN. Shows just how noisy those bins are.

  #define FIRSTBIN 3                            // The first 3 bins are garbage.
  #define LASTBIN 255                           // Don't use the highest bins, as they're (almost) a mirror of the first 256.

  float maxVal = 512;                           // Kind of a guess as to the maximum output value per combined logarithmic bins.

  for (int i=0; i<SEGLEN; i++) {

    uint16_t startBin = FIRSTBIN+i*LASTBIN/SEGLEN;        // This is the START bin for this particular pixel.
    uint16_t   endBin = FIRSTBIN+(i+1)*LASTBIN/SEGLEN;    // This is the END bin for this particular pixel.

    double sumBin = 0;

    for (int j=startBin; j<=endBin; j++) {
      sumBin += (fftBin[j] < soundSquelch*6) ? 0 : fftBin[j];  // We need some sound temporary squelch for fftBin, because we didn't do it for the raw bins in audio_reactive.h
    }

    sumBin = sumBin/(endBin-startBin+1);                  // Normalize it.
    sumBin = sumBin * (i+5) / (endBin-startBin+5);        // Disgusting frequency adjustment calculation. Lows were too bright. Am open to quick 'n dirty alternatives.

    sumBin = sumBin * 8;                                  // Need to use the 'log' version for this.

    if (sumBin > maxVal) sumBin = maxVal;                 // Make sure our bin isn't higher than the max . . which we capped earlier.

    uint8_t bright = constrain(mapf(sumBin, 0, maxVal, 0, 255),0,255);  // Map the brightness in relation to maxVal and crunch to 8 bits.

    setPixelColor(i, color_blend(SEGCOLOR(1), color_from_palette(i*8+millis()/50, false, PALETTE_SOLID_WRAP, 0), bright));  // 'i' is just an index in the palette. The FFT value, bright, is the intensity.

  } // for i

  return FRAMETIME;
} // mode_binmap()


////////////////////
//   **Freqmap    //
////////////////////

uint16_t WS2812FX::mode_freqmap(void) {                   // Map FFT_MajorPeak to SEGLEN. Would be better if a higher framerate.
  // Start frequency = 60 Hz and log10(60) = 1.78
  // End frequency = 5120 Hz and lo10(5120) = 3.71

  fade_out(SEGMENT.speed);

  uint16_t locn = (log10(FFT_MajorPeak) - 1.78) * (float)SEGLEN/(3.71-1.78);  // log10 frequency range is from 1.78 to 3.71. Let's scale to SEGLEN.
  uint16_t pixCol = (log10((int)FFT_MajorPeak) - 1.78) * 255.0/(3.71-1.78);   // Scale log10 of frequency values to the 255 colour index.
  uint16_t bright = (int)FFT_Magnitude>>7;

  setPixelColor(locn, color_blend(SEGCOLOR(1), color_from_palette(SEGMENT.intensity+pixCol, false, PALETTE_SOLID_WRAP, 0), bright));

  return FRAMETIME;
} // mode_freqmap()


///////////////////////
//   **Freqmatrix    //
///////////////////////

uint16_t WS2812FX::mode_freqmatrix(void) {                // Freqmatrix. By Andreas Pleschung.

   uint8_t secondHand = micros()/(256-SEGMENT.speed)/500 % 16;

  if(SEGENV.aux0 != secondHand) {
    SEGENV.aux0 = secondHand;

    uint32_t *leds = ledData;

    double sensitivity = mapf(SEGMENT.fft3, 1, 255, 1, 10);
    int pixVal = sampleAgc * SEGMENT.intensity / 256 * sensitivity;
    if (pixVal > 255) pixVal = 255;

    double intensity = map(pixVal, 0, 255, 0, 100) / 100.0;  // make a brightness from the last avg

    CRGB color = 0;
    CHSV c;

    if (FFT_MajorPeak > 5120) FFT_MajorPeak = 0;
      // MajorPeak holds the freq. value which is most abundant in the last sample.
      // With our sampling rate of 10240Hz we have a usable freq range from roughtly 80Hz to 10240/2 Hz
      // we will treat everything with less than 65Hz as 0
      //Serial.printf("%5d ", FFT_MajorPeak, 0);
    if (FFT_MajorPeak < 80) {
      color = CRGB::Black;
    } else {
      int upperLimit = 20 * SEGMENT.fft2;
      int lowerLimit = 2 * SEGMENT.fft1;
      int i =  map(FFT_MajorPeak, lowerLimit, upperLimit, 0, 255);
      uint16_t b = 255 * intensity;
      if (b > 255) b=255;
      c = CHSV(i, 240, (uint8_t)b);
    }

    // Serial.println(color);
    leds[0] =  (c.h << 16) + (c.s << 8)  + (c.v );

    // shift the pixels one pixel up
    for (int i = SEGLEN; i > 0; i--) {                    // Move up
      leds[i] = leds[i-1];
    }

    //fadeval = fade;

    // DISPLAY ARRAY
    for (int i= 0; i < SEGLEN; i++) {
      c.h = (leds[i] >> 16) & 0xFF;
      c.s = (leds[i] >> 8) &0xFF;
      c.v = leds[i] & 0xFF;
      color = c;                                          // implicit conversion to RGB supplied by FastLED
      setPixelColor(i, color.red, color.green, color.blue);
    }
  }

  return FRAMETIME;
} // mode_freqmatrix()


//////////////////////
//   **Freqpixels   //
//////////////////////

// Start frequency = 60 Hz and log10(60) = 1.78
// End frequency = 5120 Hz and lo10(5120) = 3.71

//  SEGMENT.speed select faderate
//  SEGMENT.intensity select colour index


uint16_t WS2812FX::mode_freqpixels(void) {                // Freqpixel. By Andrew Tuline.

  uint16_t fadeRate = 2*SEGMENT.speed - SEGMENT.speed*SEGMENT.speed/255;    // Get to 255 as quick as you can.
  fade_out(fadeRate);

  uint16_t locn = random16(0,SEGLEN);
  uint8_t pixCol = (log10((int)FFT_MajorPeak) - 1.78) * 255.0/(3.71-1.78);  // Scale log10 of frequency values to the 255 colour index.

  setPixelColor(locn, color_blend(SEGCOLOR(1), color_from_palette(SEGMENT.intensity+pixCol, false, PALETTE_SOLID_WRAP, 0), (int)FFT_Magnitude>>8));

  return FRAMETIME;
} // mode_freqpixels()


//////////////////////
//   **Freqwave     //
//////////////////////

// Assign a color to the central (starting pixels) based on the predominant frequencies and the volume. The color is being determined by mapping the MajorPeak from the FFT
// and then mapping this to the HSV color circle. Currently we are sampling at 10240 Hz, so the highest frequency we can look at is 5120Hz.
//
// SEGMENT.fft1: the lower cut off point for the FFT. (many, most time the lowest values have very little information since they are FFT conversion artifacts. Suggested value is close to but above 0
// SEGMENT.fft2: The high cut off point. This depends on your sound profile. Most music looks good when this slider is between 50% and 100%.
// SEGMENT.fft3: "preamp" for the audio signal for audio10.
//
// I suggest that for this effect you turn the brightness to 95%-100% but again it depends on your soundprofile you find yourself in.
// Instead of using colorpalettes, This effect works on the HSV color circle with red being the lowest frequency
//
// As a compromise between speed and accuracy we are currently sampling with 10240Hz, from which we can then determine with a 512bin FFT our max frequency is 5120Hz.
// Depending on the music stream you have you might find it useful to change the frequency mapping.

uint16_t WS2812FX::mode_freqwave(void) {                  // Freqwave. By Andreas Pleschung.

// Instead of using colorpalettes, This effect works on the HSV color circle with red being the lowest frequency
//
// As a compromise between speed and accuracy we are currently sampling with 10240Hz, from which we can then determine with a 512bin FFT our max frequency is 5120Hz.
// Depending on the music stream you have you might find it useful to change the frequency mapping.

  uint8_t secondHand = micros()/(256-SEGMENT.speed)/500 % 16;

//  uint8_t secondHand = millis()/(256-SEGMENT.speed) % 10;
  if(SEGENV.aux0 != secondHand) {
    SEGENV.aux0 = secondHand;

    uint32_t* leds = ledData;

    //uint8_t fade = SEGMENT.fft3;
    //uint8_t fadeval;

    double sensitivity = mapf(SEGMENT.fft3, 1, 255, 1, 10);
    int pixVal = sampleAvg * SEGMENT.intensity / 256 * sensitivity;
    if (pixVal > 255) pixVal = 255;

    double intensity = map(pixVal, 0, 255, 0, 100) / 100.0;  // make a brightness from the last avg

    CRGB color = 0;
    CHSV c;

    if (FFT_MajorPeak > 5120) FFT_MajorPeak = 0;
      // MajorPeak holds the freq. value which is most abundant in the last sample.
      // With our sampling rate of 10240Hz we have a usable freq range from roughtly 80Hz to 10240/2 Hz
      // we will treat everything with less than 65Hz as 0
      //Serial.printf("%5d ", FFT_MajorPeak, 0);
    if (FFT_MajorPeak < 80) {
      color = CRGB::Black;
    } else {
      int upperLimit = 20 * SEGMENT.fft2;
      int lowerLimit = 2 * SEGMENT.fft1;
      int i =  map(FFT_MajorPeak, lowerLimit, upperLimit, 0, 255);
      uint16_t b = 255 * intensity;
      if (b > 255) b=255;
      c = CHSV(i, 240, (uint8_t)b);
    }

    // Serial.println(color);
    leds[SEGLEN/2] =  (c.h << 16) + (c.s << 8)  + (c.v );

// shift the pixels one pixel outwards
    for (int i = SEGLEN; i > SEGLEN/2; i--) {             // Move to the right.
      leds[i] = leds[i-1];
    }
    for (int i = 0; i < SEGLEN/2; i++) {                  // Move to the left.
      leds[i] = leds[i+1];
    }

    // DISPLAY ARRAY
    for (int i= 0; i < SEGLEN; i++) {
      c.h = (leds[i] >> 16) & 0xFF;
      c.s = (leds[i] >> 8) &0xFF;
      c.v = leds[i] & 0xFF;
      color = c;                                          // implicit conversion to RGB supplied by FastLED
      setPixelColor(i, color.red, color.green, color.blue);
    }
  }

  return FRAMETIME;
} // mode_freqwave()


///////////////////////
//    **Gravfreq     //
///////////////////////

uint16_t WS2812FX::mode_gravfreq(void) {                  // Gravfreq. By Andrew Tuline.

  static int topLED;
  static int gravityCounter = 0;

  fade_out(240);

  sampleAvg = sampleAvg * SEGMENT.intensity / 255;

  int tempsamp = constrain(sampleAvg*2,0,SEGLEN/2-1);     // Keep the sample from overflowing.
  uint8_t gravity = 8 - SEGMENT.speed/32;

  for (int i=0; i<tempsamp; i++) {

    uint8_t index = (log10((int)FFT_MajorPeak) - (3.71-1.78)) * 255;

    setPixelColor(i+SEGLEN/2, color_blend(SEGCOLOR(1), color_from_palette(index, false, PALETTE_SOLID_WRAP, 0), 255));
    setPixelColor(SEGLEN/2-i, color_blend(SEGCOLOR(1), color_from_palette(index, false, PALETTE_SOLID_WRAP, 0), 255));
  }

  if (tempsamp >= topLED)
    topLED = tempsamp;
  else if (gravityCounter % gravity == 0)
    topLED--;

  if (topLED > 0) {
    setPixelColor(topLED+SEGLEN/2, CRGB::Gray);
    setPixelColor(SEGLEN/2-topLED, CRGB::Gray);
  }
  gravityCounter = (gravityCounter + 1) % gravity;

  return FRAMETIME;
} // mode_gravfreq()


//////////////////////
//   **Noisemove    //
//////////////////////

uint16_t WS2812FX::mode_noisemove(void) {                 // Noisemove.    By: Andrew Tuline

  fade_out(224);                                          // Just in case something doesn't get faded.

  uint8_t numBins = map(SEGMENT.intensity,0,255,0,16);    // Map slider to fftResult bins.

  for (int i=0; i<numBins; i++) {                         // How many active bins are we using.
    uint16_t locn = inoise16(millis()*SEGMENT.speed+i*50000, millis()*SEGMENT.speed);   // Get a new pixel location from moving noise.

    locn = map(locn,7500,58000,0,SEGLEN-1);               // Map that to the length of the strand, and ensure we don't go over.
    locn = locn % (SEGLEN - 1);                           // Just to be bloody sure.

    setPixelColor(locn, color_blend(SEGCOLOR(1), color_from_palette(i*64, false, PALETTE_SOLID_WRAP, 0), fftResult[i]*4));

  }

  return FRAMETIME;
} // mode_noisemove()


///////////////////////
//   **Waterfall     //
///////////////////////

// Combines peak detection with FFT_MajorPeak and FFT_Magnitude.

uint16_t WS2812FX::mode_waterfall(void) {                   // Waterfall. By: Andrew Tuline

  CRGB *leds = (CRGB*) ledData;
  if (SEGENV.call == 0) fill_solid(leds,SEGLEN, 0);

  uint8_t secondHand = micros() / (256-SEGMENT.speed)/500 + 1 % 16;

  if (SEGENV.aux0 != secondHand) {                        // Triggered millis timing.
    SEGENV.aux0 = secondHand;

    uint8_t pixCol = (log10((int)FFT_MajorPeak) - 2.26) * 177;  // log10 frequency range is from 2.26 to 3.7. Let's scale accordingly.

    if (samplePeak) {
      leds[SEGLEN-1] = CHSV(92,92,92);
    } else {
      leds[SEGLEN-1] = color_blend(SEGCOLOR(1), color_from_palette(pixCol+SEGMENT.intensity, false, PALETTE_SOLID_WRAP, 0), (int)FFT_Magnitude>>8);
    }
      for (int i=0; i<SEGLEN-1; i++) leds[i] = leds[i+1];
  }

  setPixels(leds);
  return FRAMETIME;
} // mode_waterfall()


/////////////////////////
//   **DJLight         //
/////////////////////////

uint16_t WS2812FX::mode_DJLight(void) {                   // Written by ??? Adapted by Will Tatam.
  int NUM_LEDS = SEGLEN;                                  // aka SEGLEN
  int mid = NUM_LEDS / 2;
  CRGB *leds = (CRGB* )ledData;

  uint8_t secondHand = micros()/(256-SEGMENT.speed)/500+1 % 64;

  if (SEGENV.aux0 != secondHand) {                        // Triggered millis timing.
    SEGENV.aux0 = secondHand;

    leds[mid] = CRGB(fftResult[16]/2, fftResult[5]/2, fftResult[0]/2);
    leds[mid].fadeToBlackBy(map(fftResult[1*4], 0, 255, 255, 10)); // TODO - Update

    //move to the left
    for (int i = NUM_LEDS - 1; i > mid; i--) {
      leds[i] = leds[i - 1];
    }
    // move to the right
    for (int i = 0; i < mid; i++) {
      leds[i] = leds[i + 1];
    }
  }

  setPixels(leds);
  return FRAMETIME;
} // mode_DJLight()



//////////////////////////////////////////////
//       START of 2D REACTIVE ROUTINES      //
//////////////////////////////////////////////

/////////////////////////
//     **2D GEQ        //
/////////////////////////

uint16_t WS2812FX::mode_2DGEQ(void) {                     // By Will Tatam.

  fade_out(224);                                          // Just in case something doesn't get faded.

  CRGB *leds = (CRGB*) ledData;
  fadeToBlackBy(leds, SEGLEN, SEGMENT.speed);

  int NUMB_BANDS = map(SEGMENT.fft3, 0, 255, 1, 16);
  int barWidth = (matrixWidth / NUMB_BANDS);
  int bandInc = 1;
  if(barWidth == 0) {
    // Matrix narrower than fft bands
    barWidth = 1;
    bandInc = (NUMB_BANDS / matrixWidth);
  }

  int b = 0;
  for (int band = 0; band < NUMB_BANDS; band += bandInc) {
    int count = map(fftResult[band], 0, 255, 0, matrixHeight);
    for (int w = 0; w < barWidth; w++) {
      int xpos = (barWidth * b) + w;
      for (int i = 0; i <=  matrixHeight; i++) {
        if (i <= count) {
        leds[XY(xpos, i)] = color_blend(SEGCOLOR(1), color_from_palette((band * 35), false, PALETTE_SOLID_WRAP, 0), 255);
        }
      }
    }
    b++;
  }

  setPixels(leds);
  return FRAMETIME;
} // mode_2DGEQ()


/////////////////////////
//  **2D Funky plank   //
/////////////////////////

uint16_t WS2812FX::mode_2DFunkyPlank(void) {              // Written by ??? Adapted by Will Tatam.

  CRGB *leds = (CRGB*) ledData;

  int NUMB_BANDS = map(SEGMENT.fft3, 0, 255, 1, 16);
  int barWidth = (matrixWidth / NUMB_BANDS);
  int bandInc = 1;
  if(barWidth == 0) {
    // Matrix narrower than fft bands
    barWidth = 1;
    bandInc = (NUMB_BANDS / matrixWidth);
  }

  uint8_t secondHand = micros()/(256-SEGMENT.speed)/500+1 % 64;

  if (SEGENV.aux0 != secondHand) {                        // Triggered millis timing.
    SEGENV.aux0 = secondHand;

    // display values of
    int b = 0;
    for (int band = 0; band < NUMB_BANDS; band += bandInc) {
      int hue = fftResult[band];
      int v = map(fftResult[band], 0, 255, 10, 255);
//     if(hue > 0) Serial.printf("Band: %u Value: %u\n", band, hue);
     for (int w = 0; w < barWidth; w++) {
         int xpos = (barWidth * b) + w;
         leds[XY(xpos, 0)] = CHSV(hue, 255, v);
      }
      b++;
    }

    // Update the display:
    for (int i = (matrixHeight - 1); i > 0; i--) {
      for (int j = (matrixWidth - 1); j >= 0; j--) {
        int src = XY(j, (i - 1));
        int dst = XY(j, i);
        leds[dst] = leds[src];
      }
    }

  }

  setPixels(leds);
  return FRAMETIME;
} // mode_2DFunkyPlank


/////////////////////////
//   **2D CenterBars   //
/////////////////////////

uint16_t WS2812FX::mode_2DCenterBars(void) {              // Written by Scott Marley Adapted by  Spiro-C..

  CRGB *leds = (CRGB*) ledData;
  fadeToBlackBy(leds, SEGLEN, SEGMENT.speed);

  int NUMB_BANDS = map(SEGMENT.fft3, 0, 255, 1, 16);
  int barWidth = (matrixWidth / NUMB_BANDS);
  int bandInc = 1;
  if(barWidth == 0) {
    // Matrix narrower than fft bands
    barWidth = 1;
    bandInc = (NUMB_BANDS / matrixWidth);
  }

  int b = 0;
  for (int band = 0; band < NUMB_BANDS; band += bandInc) {
    int hight = map(fftResult[band], 0, 255, 0, matrixHeight);
    if (hight % 2 == 0) hight--;
    int yStart = ((matrixHeight - hight) / 2 );
    for (int w = 0; w < barWidth; w++) {
      int x = (barWidth * b) + w;
      for (int y = yStart; y <= (yStart + hight); y++) {
//        leds[XY(x, y)] = CHSV((band * 35), 255, 255);
         leds[XY(x, y)] = color_blend(SEGCOLOR(1), color_from_palette((band * 35), false, PALETTE_SOLID_WRAP, 0), 255);
      }
    }
    b++;
  }
  setPixels(leds);
  return FRAMETIME;
} // mode_2DCenterBars()


//////////////////////////////////////////////
//     START of 2D NON-REACTIVE ROUTINES    //
//////////////////////////////////////////////

static uint16_t x = 0;
static uint16_t y = 0;
static uint16_t z = 0;
static int speed2D = 20;

// uint8_t colorLoop = 1;

// Scale determines how far apart the pixels in our noise matrix are.  Try
// changing these values around to see how it affects the motion of the display.  The
// higher the value of scale, the more "zoomed out" the noise iwll be.  A value
// of 1 will be so zoomed in, you'll mostly see solid colors.
static int scale_2d = 30; // scale is set dynamically once we've started up

// blur1d: one-dimensional blur filter. Spreads light to 2 line neighbors.
// blur2d: two-dimensional blur filter. Spreads light to 8 XY neighbors.
//
//           0 = no spread at all
//          64 = moderate spreading
//         172 = maximum smooth, even spreading
//
//         173..255 = wider spreading, but increasing flicker
//
//         Total light is NOT entirely conserved, so many repeated
//         calls to 'blur' will also result in the light fading,
//         eventually all the way to black; this is by design so that
//         it can be used to (slowly) clear the LEDs to black.
void WS2812FX::blur1d( CRGB* leds, uint16_t numLeds, fract8 blur_amount)
{
    uint8_t keep = 255 - blur_amount;
    uint8_t seep = blur_amount >> 1;
    CRGB carryover = CRGB::Black;
    for( uint16_t i = 0; i < numLeds; i++) {
        CRGB cur = leds[i];
        CRGB part = cur;
        part.nscale8( seep);
        cur.nscale8( keep);
        cur += carryover;
        if( i) leds[i-1] += part;
        leds[i] = cur;
        carryover = part;
    }
}

void WS2812FX::blur2d( CRGB* leds, uint8_t width, uint8_t height, fract8 blur_amount)
{
    blurRows(leds, width, height, blur_amount);
    blurColumns(leds, width, height, blur_amount);
}

// blurRows: perform a blur1d on every row of a rectangular matrix
void WS2812FX::blurRows( CRGB* leds, uint8_t width, uint8_t height, fract8 blur_amount)
{
    for( uint8_t row = 0; row < height; row++) {
        CRGB* rowbase = leds + (row * width);
        blur1d( rowbase, width, blur_amount);
    }
}

// blurColumns: perform a blur1d on each column of a rectangular matrix
void WS2812FX::blurColumns(CRGB* leds, uint8_t width, uint8_t height, fract8 blur_amount)
{
    // blur columns
    uint8_t keep = 255 - blur_amount;
    uint8_t seep = blur_amount >> 1;
    for( uint8_t col = 0; col < width; col++) {
        CRGB carryover = CRGB::Black;
        for( uint8_t i = 0; i < height; i++) {
            CRGB cur = leds[XY(col,i)];
            CRGB part = cur;
            part.nscale8( seep);
            cur.nscale8( keep);
            cur += carryover;
            if( i) leds[XY(col,i-1)] += part;
            leds[XY(col,i)] = cur;
            carryover = part;
        }
    }
}

// Set 'matrixSerpentine' to false if your pixels are
// laid out all running the same way, like this:
//
//     0 >  1 >  2 >  3 >  4
//                         |
//     .----<----<----<----'
//     |
//     5 >  6 >  7 >  8 >  9
//                         |
//     .----<----<----<----'
//     |
//    10 > 11 > 12 > 13 > 14
//                         |
//     .----<----<----<----'
//     |
//    15 > 16 > 17 > 18 > 19
//
// Set 'matrixSerpentine' to true if your pixels are
// laid out back-and-forth, like this:
//
//     0 >  1 >  2 >  3 >  4
//                         |
//                         |
//     9 <  8 <  7 <  6 <  5
//     |
//     |
//    10 > 11 > 12 > 13 > 14
//                        |
//                        |
//    19 < 18 < 17 < 16 < 15
//
// Bonus vocabulary word: anything that goes one way
// in one row, and then backwards in the next row, and so on
// is call "boustrophedon", meaning "as the ox plows."


// This function will return the right 'led index number' for
// a given set of X and Y coordinates on your matrix.
// IT DOES NOT CHECK THE COORDINATE BOUNDARIES.
// That's up to you.  Don't pass it bogus values.
//
// Use the "XY" function like this:
//
//    for( uint8_t x = 0; x < matrixWidth; x++) {
//      for( uint8_t y = 0; y < matrixHeight; y++) {
//
//        // Here's the x, y to 'led index' in action:
//        leds[ XY( x, y) ] = CHSV( random8(), 255, 255);
//
//      }
//    }
//
//
uint16_t WS2812FX::XY( int x, int y) {

uint16_t i;

  if( matrixSerpentine == false) {
    i = (y * matrixWidth) + x;
  }

  if( matrixSerpentine == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (matrixWidth - 1) - x;
      i = (y * matrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * matrixWidth) + x;
    }
  }

  return i;
} // XY()


//////////////////////
//    2D Plasma     //
//////////////////////

// Effect speed slider determines the speed the 'plasma' wafts
// fft1 slider above 1/2 will shift the colors
// fft2 slider == scale (how far away are we from the plasma)

uint16_t WS2812FX::mode_2Dplasma(void) {                  // By Andreas Pleschutznig. A work in progress.

  if (matrixWidth * matrixHeight > SEGLEN) {fade_out(224); return FRAMETIME;}                                 // No, we're not going to overrun the segment.

  static uint8_t ihue=0;
  uint8_t index;
  uint8_t bri;
  static unsigned long prevMillis;
  unsigned long curMillis = millis();

  if ((curMillis - prevMillis) >= ((256-SEGMENT.speed) >>2)) {
    prevMillis = curMillis;
    speed2D = SEGMENT.speed;
    scale_2d = SEGMENT.fft2;

    uint32_t *noise = ledData;                            // we use the set aside storage array for FFT routines to store temporary 2D data
    uint8_t MAX_DIMENSION = ((matrixWidth>matrixHeight) ? matrixWidth : matrixHeight);

    // If we're runing at a low "speed", some 8-bit artifacts become visible
    // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
    // The amount of data smoothing we're doing depends on "speed".

    uint8_t dataSmoothing = 0;
    if( speed2D < 50) {
      dataSmoothing = 200 - (speed2D * 4);
      }

    for(int i = 0; i < MAX_DIMENSION; i++) {
      int ioffset = scale_2d * i;
      for(int j = 0; j < MAX_DIMENSION; j++) {
        int joffset = scale_2d * j;

        uint8_t data = inoise8(x + ioffset,y + joffset,z);

          // The range of the inoise8 function is roughly 16-238.
          // These two operations expand those values out to roughly 0..255
          // You can comment them out if you want the raw noise data.
        data = qsub8(data,16);
        data = qadd8(data,scale8(data,39));

        if( dataSmoothing ) {
          uint8_t olddata = noise[i * matrixWidth + j];
          uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
          data = newdata;
        }

        noise[i * matrixWidth + j] = data;
      }
    }

    z += speed2D;

    // apply slow drift to X and Y, just for visual variation.
    x += speed2D / 8;
    y -= speed2D / 16;

 // ---

  for(int i = 0; i < matrixWidth; i++) {
    for(int j = 0; j < matrixHeight; j++) {
      // We use the value at the (i,j) coordinate in the noise
      // array for our brightness, and the flipped value from (j,i)
      // for our pixel's index into the color palette.

      uint8_t index = noise[j * matrixWidth + i];
      uint8_t bri =   noise[i * matrixWidth + j];

      // if this palette is a 'loop', add a slowly-changing base value
      if (SEGMENT.fft1 > 128) {
        index += ihue;
      }

      // brighten up, as the color palette itself often contains the
      // light/dark dynamic range desired
      if( bri > 127 ) {
        bri = 255;
      } else {
        bri = dim8_raw( bri * 2);
      }

      CRGB color = ColorFromPalette( currentPalette, index, bri);
      setPixelColor(XY(i, j), color.red, color.green, color.blue);
      }
    }
  ihue+=1;
  }

  return FRAMETIME;
} // mode_2Dplasma()


//////////////////////////
//     2D Firenoise     //
//////////////////////////

uint16_t WS2812FX::mode_2Dfirenoise(void) {               // firenoise2d. By Andrew Tuline. Yet another short routine.

  if (matrixWidth * matrixHeight > SEGLEN) {return blink(CRGB::Red, CRGB::Black, false, false);}                 // No, we're not going to overrun the segment.

  CRGB *leds = (CRGB *)ledData;

  uint32_t xscale = 600;                                  // How far apart they are
  uint32_t yscale = 1000;                                 // How fast they move
  uint8_t indexx = 0;

  currentPalette = CRGBPalette16(  CHSV(0,255,2), CHSV(0,255,4), CHSV(0,255,8), CHSV(0, 255, 8),
                                   CHSV(0, 255, 16), CRGB::Red, CRGB::Red, CRGB::Red,
                                   CRGB::DarkOrange,CRGB::DarkOrange, CRGB::Orange, CRGB::Orange,
                                   CRGB::Yellow, CRGB::Orange, CRGB::Yellow, CRGB::Yellow);
  int a = millis();
  for (int j=0; j < matrixWidth; j++) {
    for (int i=0; i < matrixHeight; i++) {

      // This perlin fire is by Andrew Tuline
      indexx = inoise8(i*xscale+millis()/4,j*yscale*matrixWidth/255);                                             // We're moving along our Perlin map.
      leds[XY(i,j)] = ColorFromPalette(currentPalette, min(i*(indexx)>>4, 255), i*255/matrixWidth, LINEARBLEND);  // With that value, look up the 8 bit colour palette value and assign it to the current LED.

// This perlin fire is my /u/ldirko
//      leds[XY(i,j)] = ColorFromPalette (currentPalette, qsub8(inoise8 (i * 60 , j * 60+ a , a /3), abs8(j - (matrixHeight-1)) * 255 / (matrixHeight-1)), 255);

    } // for i
  } // for j

  for (int i=0; i<SEGLEN; i++) {
    setPixelColor(i, leds[i].red, leds[i].green, leds[i].blue);
  }

  return FRAMETIME;
} // mode_2Dfirenoise()


//////////////////////////////
//     2D Squared Swirl     //
//////////////////////////////

uint16_t WS2812FX::mode_2Dsquaredswirl(void) {            // By: Mark Kriegsman. https://gist.github.com/kriegsman/368b316c55221134b160
                                                          // Modifed by: Andrew Tuline
                                                          // fft3 affects the blur amount.

  if (matrixWidth * matrixHeight > SEGLEN) {return blink(CRGB::Red, CRGB::Black, false, false);}  // No, we're not going to overrun the segment.

  CRGB *leds = (CRGB *)ledData;
  const uint8_t kBorderWidth = 2;

  fadeToBlackBy(leds, SEGLEN, 24);
  // uint8_t blurAmount = dim8_raw( beatsin8(20,64,128) );  //3,64,192
  uint8_t blurAmount = SEGMENT.fft3;
  blur2d(leds, matrixWidth, matrixHeight, blurAmount);

  // Use two out-of-sync sine waves
  uint8_t  i = beatsin8(19, kBorderWidth, matrixWidth-kBorderWidth);
  uint8_t  j = beatsin8(22, kBorderWidth, matrixWidth-kBorderWidth);
  uint8_t  k = beatsin8(17, kBorderWidth, matrixWidth-kBorderWidth);
  uint8_t  m = beatsin8(18, kBorderWidth, matrixHeight-kBorderWidth);
  uint8_t  n = beatsin8(15, kBorderWidth, matrixHeight-kBorderWidth);
  uint8_t  p = beatsin8(20, kBorderWidth, matrixHeight-kBorderWidth);

  uint16_t ms = millis();

  leds[XY( i, m)] += ColorFromPalette(currentPalette, ms/29, 255, LINEARBLEND);
  leds[XY( j, n)] += ColorFromPalette(currentPalette, ms/41, 255, LINEARBLEND);
  leds[XY( k, p)] += ColorFromPalette(currentPalette, ms/73, 255, LINEARBLEND);

  for (int i=0; i<SEGLEN; i++) {
    setPixelColor(i, leds[i].red, leds[i].green, leds[i].blue);
  }

  return FRAMETIME;
} // mode_2Dsquaredswirl()


/////////////////////////
//     2D Fire2012     //
/////////////////////////

uint16_t WS2812FX::mode_2Dfire2012(void) {                // Fire2012 by Mark Kriegsman. Converted to WLED by Andrew Tuline.

  if (matrixWidth * matrixHeight > SEGLEN) {return blink(CRGB::Red, CRGB::Black, false, false);}  // No, we're not going to overrun the segment.

  CRGB *leds = (CRGB *)ledData;
  static byte *heat = (byte *)dataStore;

  const uint8_t COOLING = 50;
  const uint8_t SPARKING = 50;

  CRGBPalette16 currentPalette  = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Orange, CRGB::Yellow);

  static unsigned long prevMillis;
  unsigned long curMillis = millis();

  if ((curMillis - prevMillis) >= ((256-SEGMENT.speed) >>2)) {
    prevMillis = curMillis;
    static byte *heat = (byte *)dataStore;

    for (int mw = 0; mw < matrixWidth; mw++) {            // Move along the width of the flame

      // Step 1.  Cool down every cell a little
      for (int mh = 0; mh < matrixHeight; mh++) {
        heat[mw*matrixWidth+mh] = qsub8( heat[mw*matrixWidth+mh],  random16(0, ((COOLING * 10) / matrixHeight) + 2));
      }

      // Step 2.  Heat from each cell drifts 'up' and diffuses a little
      for (int mh = matrixHeight - 1; mh >= 2; mh--) {
        heat[mw*matrixWidth+mh] = (heat[mw*matrixWidth+mh - 1] + heat[mw*matrixWidth+mh - 2] + heat[mw*matrixWidth+mh - 2] ) / 3;
      }

      // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
      if (random8(0,255) < SPARKING ) {
        int mh = random8(3);
        heat[mw*matrixWidth+mh] = qadd8( heat[mw*matrixWidth+mh], random8(160,255) );
      }

      // Step 4.  Map from heat cells to LED colors
      for (int mh = 0; mh < matrixHeight; mh++) {
        byte colorindex = scale8( heat[mw*matrixWidth+mh], 240);
        leds[XY(mw,mh)] = ColorFromPalette(currentPalette, colorindex, 255);
      } // for mh
    } // for mw

    for (int i=0; i<SEGLEN; i++) {
      setPixelColor(i, leds[i].red, leds[i].green, leds[i].blue);
    }

  } // if millis

  return FRAMETIME;
} // mode_2Dfire2012()


/////////////////////
//      2D DNA     //
/////////////////////

uint16_t WS2812FX::mode_2Ddna(void) {         // dna originally by by ldirko at https://pastebin.com/pCkkkzcs. Updated by Preyy. WLED version by Andrew Tuline.

  if (matrixWidth * matrixHeight > SEGLEN) {return blink(CRGB::Red, CRGB::Black, false, false);}    // No, we're not going to overrun the segment.

  CRGB *leds = (CRGB *)ledData;

  fadeToBlackBy(leds, SEGLEN, 64);

  static unsigned long prevMillis;
  unsigned long curMillis = millis();

  if ((curMillis - prevMillis) >= ((256-SEGMENT.speed) >>3)) {
    prevMillis = curMillis;

  for(int i = 0; i < matrixHeight; i++) {
      leds[XY(beatsin8(10, 0, matrixWidth-1, 0, i*4), i)] = ColorFromPalette(currentPalette, i*5+millis()/17, beatsin8(5, 55, 255, 0, i*10), LINEARBLEND);
      leds[XY(beatsin8(10, 0, matrixWidth-1, 0, i*4+128), i)] = ColorFromPalette(currentPalette,i*5+128+millis()/17, beatsin8(5, 55, 255, 0, i*10+128), LINEARBLEND);        // 180 degrees (128) out of phase
  }

  blur2d(leds, matrixWidth, matrixHeight, 2);

   for (int i=0; i<SEGLEN; i++) {
      setPixelColor(i, leds[i].red, leds[i].green, leds[i].blue);
   }

  } // if millis

  return FRAMETIME;
} // mode_2Ddna()


///////////////////////
//    2D Matrix      //
///////////////////////

uint16_t WS2812FX::mode_2Dmatrix(void) {                  // Matrix2D. By Jeremy Williams. Adapted by Andrew Tuline.

  if (matrixWidth * matrixHeight > SEGLEN) {return blink(CRGB::Red, CRGB::Black, false, false);}    // No, we're not going to overrun the segment.

  CRGB *leds = (CRGB* )ledData;

  static unsigned long prevMillis;
  unsigned long curMillis = millis();

  if (SEGENV.call == 0) fill_solid(leds,SEGLEN, 0);

  if ((curMillis - prevMillis) >= ((256-SEGMENT.speed) >>2)) {
    prevMillis = curMillis;

    if (SEGMENT.fft3 < 128) {                                               // check for orientation, slider in first quarter, default orientation
        for (int16_t row=matrixHeight-1; row>=0; row--) {
            for (int16_t col=0; col<matrixWidth; col++) {
                if (leds[XY(col, row)] == CRGB(175,255,175)) {
                    leds[XY(col, row)] = CRGB(27,130,39);         // create trail
                    if (row < matrixHeight-1) leds[XY(col, row+1)] = CRGB(175,255,175);
                }
            }
        }
    } else if (SEGMENT.fft3 >= 128)   {                   // second quadrant
        for (int16_t row=matrixHeight-1; row>=0; row--) {
                    for (int16_t col=matrixWidth-1; col >= 0; col--) {
                        if (leds[XY(col, row)] == CRGB(175,255,175)) {
                            leds[XY(col, row)] = CRGB(27,130,39);   // create trail
                            if (row < matrixHeight-1) leds[XY(col+1, row)] = CRGB(175,255,175);
                        }
                    }
                }
      }
    // fade all leds
    for(int i = 0; i < SEGLEN; i++) {
      if (leds[i].g != 255) leds[i].nscale8(192);         // only fade trail
    }

    // check for empty screen to ensure code spawn
    bool emptyScreen = true;
    for(int i = 0; i < SEGLEN; i++) {
      if (leds[i])
      {
        emptyScreen = false;
        break;
      }
    }

    // spawn new falling code
    if (SEGMENT.fft3 < 128) {
        if (random8(3) == 0 || emptyScreen) {               // lower number == more frequent spawns
          uint8_t spawnX = random8(matrixWidth);
          leds[XY(spawnX, 0)] = CRGB(175,255,175 );
        }
    } else if (SEGMENT.fft3 >= 128) {
        if (random8(3) == 0 || emptyScreen) {               // lower number == more frequent spawns
          uint8_t spawnX = random8(matrixHeight);
          leds[XY(0, spawnX)] = CRGB(175,255,175 );
          }
    }

   for (int i=0; i<SEGLEN; i++) {
      setPixelColor(i, leds[i].red, leds[i].green, leds[i].blue);
   }
  } // if millis

  return FRAMETIME;
} // mode_2Dmatrix()


/////////////////////////
//     2D Meatballs    //
/////////////////////////

uint16_t WS2812FX::mode_2Dmeatballs(void) {   // Metaballs by Stefan Petrick. Cannot have one of the dimensions be 2 or less. Adapted by Andrew Tuline.

  if (matrixWidth * matrixHeight > SEGLEN) {return blink(CRGB::Red, CRGB::Black, false, false);}    // No, we're not going to overrun the segment.

  CRGB *leds = (CRGB* )ledData;

  float speed = 1;

  // get some 2 random moving points
  uint8_t x2 = inoise8(millis() * speed, 25355, 685 ) / 16;
  uint8_t y2 = inoise8(millis() * speed, 355, 11685 ) / 16;

  uint8_t x3 = inoise8(millis() * speed, 55355, 6685 ) / 16;
  uint8_t y3 = inoise8(millis() * speed, 25355, 22685 ) / 16;

  // and one Lissajou function
  uint8_t x1 = beatsin8(23 * speed, 0, 15);
  uint8_t y1 = beatsin8(28 * speed, 0, 15);

  for (uint8_t y = 0; y < matrixHeight; y++) {
    for (uint8_t x = 0; x < matrixWidth; x++) {

      // calculate distances of the 3 points from actual pixel
      // and add them together with weightening
      uint8_t  dx =  abs(x - x1);
      uint8_t  dy =  abs(y - y1);
      uint8_t dist = 2 * sqrt((dx * dx) + (dy * dy));

      dx =  abs(x - x2);
      dy =  abs(y - y2);
      dist += sqrt((dx * dx) + (dy * dy));

      dx =  abs(x - x3);
      dy =  abs(y - y3);
      dist += sqrt((dx * dx) + (dy * dy));

      // inverse result
      byte color = 1000 / dist;

      // map color between thresholds
      if (color > 0 and color < 60) {
        leds[XY(x, y)] = ColorFromPalette(currentPalette, color * 9, 255);
      } else {
        leds[XY(x, y)] = ColorFromPalette(currentPalette, 0, 255);
      }
        // show the 3 points, too
        leds[XY(x1,y1)] = CRGB(255, 255,255);
        leds[XY(x2,y2)] = CRGB(255, 255,255);
        leds[XY(x3,y3)] = CRGB(255, 255,255);
    }
  }

   for (int i=0; i<SEGLEN; i++) {
      setPixelColor(i, leds[i].red, leds[i].green, leds[i].blue);
   }

  return FRAMETIME;
} // mode_2Dmeatballs()


////////////////////////////////
//  **FFT_test                //
////////////////////////////////

// This is our internal testing routine, which has been exposed for use with debugging.

// Some variables
// SEGENV.aux0  - A uint16_t persistent value
// SEGMENT.fft2 - A unit_t slider
// SEGMENT.fft3 - A uint8_t slider
// FFT_MajorPeak
// FFT_Magnitude


// uint16_t WS2812FX::fft_test() {

// Configure decay for volume based variable sample
/*  if (!SEGENV.allocateData(sizeof(float))) return mode_static();
  float* expAdjF = reinterpret_cast<float*>(SEGENV.data);

  float weightingF = (float)(SEGMENT.fft3+40)/1000.0;            // Take 0 to 255 from slider and recalculate to be << 1.0, and with a reasonable range of decay throughout.

  if ((float)sample > *expAdjF) {
    *expAdjF = (float)sample;
  } else {
    *expAdjF = (weightingF * (float)sample + (1.0-weightingF) * *expAdjF);
  }

//  Serial.print("Sample:\t"); Serial.print(sample);
  Serial.print(",expAdjF:\t"); Serial.print(*expAdjF,3);
  Serial.println(" ");
*/



// Configure decay for FFT based variable fftResult[16]
/*  if (!SEGENV.allocateData(sizeof(float) * 16)) return mode_static();
  float* expAdjF = reinterpret_cast<float*>(SEGENV.data);
  float weightingF = (float)(SEGMENT.fft3+40)/1000.0;            // Take 0 to 255 and recalculate to be << 1.0

  for (int i = 0; i < 16; i++) {

   if ((float)sample > *expAdjF[i]) {
    *expAdjF[i] = (float)sample;
  } else {
    expAdjF[i] = (weightingF * (float)sample + (1.0-weightingF) * expAdjF[i]);
  }

    Serial.print(fftResult[i]);
    Serial.print("\t"); Serial.print(expAdjF[i],3);Serial.print("\t");
  }
  Serial.println(" "); Serial.println(" ");
*/

// Testing fftResult values to see if/how we can normalize results.
/*
// Select a bin to display
uint8_t binVal = map(SEGMENT.fft3,0,255,0,15);

// Display smoothed results of selected bin.
SEGENV.aux0 = ((SEGENV.aux0 * 15) + fftResult[binVal]) / 16;


Serial.print("binVal:\t"); Serial.print(binVal);
//Serial.print(",fftResult:\t"); Serial.print(fftResult[binVal]);
Serial.print(",Avg:\t"); Serial.print(SEGENV.aux0);
Serial.println(" ");
*/

//    for (int i=0; i < 16; i++) {
//        fftResult[i] = fftResult[i]-(float)soundSquelch*(float)linearNoise[i]/4.0 <= 0? 0 : fftResult[i]-(float)soundSquelch*(float)linearNoise[i]/4.0;
//    }

//  return FRAMETIME;
//} //
