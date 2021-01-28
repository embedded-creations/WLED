#pragma once

// This example effect's name is "FL_TWINKLES" - `#define USERFX1 FL_TWINKLES` will assign this FL_TWINKLES effect to UserFX slot number 1

// https://gist.github.com/kriegsman/5408ecd397744ba0393e
// Note: this effect is already in WLED with a different implementation (mode_colortwinkle), which can be used for comparison
uint16_t mode_fl_twinkles(void) {
  FL_ALLOC_WITH_1_ARRAY_XY(

    , uint8_t, directionFlags, (NUM_LEDS+7) / 8);

  enum { GETTING_DARKER = 0, GETTING_BRIGHTER = 1 };

  const int STARTING_BRIGHTNESS = 64;
  const int FADE_IN_SPEED       = 32;
  const int FADE_OUT_SPEED      = 20;
  const int DENSITY            = 255;

  FL_FUNCTIONS_BEGIN // vvvvvvvvv Local Functions go here
    void brightenOrDarkenEachPixel( fract8 fadeUpAmount, fract8 fadeDownAmount)
    {
     for( uint16_t i = 0; i < NUM_LEDS; i++) {
        if( getPixelDirection(i) == GETTING_DARKER) {
          // This pixel is getting darker
          leds[i] = makeDarker( leds[i], fadeDownAmount);
        } else {
          // This pixel is getting brighter
          leds[i] = makeBrighter( leds[i], fadeUpAmount);
          // now check to see if we've maxxed out the brightness
          if( leds[i].r == 255 || leds[i].g == 255 || leds[i].b == 255) {
            // if so, turn around and start getting darker
            setPixelDirection(i, GETTING_DARKER);
          }
        }
      }
    }

    CRGB makeBrighter( const CRGB& color, fract8 howMuchBrighter) 
    {
      CRGB incrementalColor = color;
      incrementalColor.nscale8( howMuchBrighter);
      return color + incrementalColor;
    }

    CRGB makeDarker( const CRGB& color, fract8 howMuchDarker) 
    {
      CRGB newcolor = color;
      newcolor.nscale8( 255 - howMuchDarker);
      return newcolor;
    }

    bool getPixelDirection( uint16_t i) {
      uint16_t index = i / 8;
      uint8_t  bitNum = i & 0x07;
      // using Arduino 'bitRead' function; expanded code below
      return bitRead( FL_STATICVAR(directionFlags)[index], bitNum);
      // uint8_t  andMask = 1 << bitNum;
      // return (directionFlags[index] & andMask) != 0;
    }

    void setPixelDirection( uint16_t i, bool dir) {
      uint16_t index = i / 8;
      uint8_t  bitNum = i & 0x07;
      // using Arduino 'bitWrite' function; expanded code below
      bitWrite( FL_STATICVAR(directionFlags)[index], bitNum, dir);
      //  uint8_t  orMask = 1 << bitNum;
      //  uint8_t andMask = 255 - orMask;
      //  uint8_t value = directionFlags[index] & andMask;
      //  if( dir ) {
      //    value += orMask;
      //  }
      //  directionFlags[index] = value;
    }
  FL_FUNCTIONS_END // ^^^^^^^^^ Local Functions go here

  // Make each pixel brighter or darker, depending on
  // its 'direction' flag.
  FL_FUNCTION(brightenOrDarkenEachPixel)( FADE_IN_SPEED, FADE_OUT_SPEED);
  
  // Now consider adding a new random twinkle
  if( random8() < DENSITY ) {
    int pos = random16(NUM_LEDS);
    if( !leds[pos]) {
      leds[pos] = ColorFromPalette( currentPalette, random8(), STARTING_BRIGHTNESS, NOBLEND);
      FL_FUNCTION(setPixelDirection)(pos, GETTING_BRIGHTER);
    }
  }

  END_FASTLED_COMPATIBILITY();

  return FRAMETIME;
}

// set this to the name of your effect matching the name of the function defined above
#define FL_TWINKLES_MODE_NAME            mode_fl_twinkles

// set this to the name of your effect as you want it shown in the web interface
#define FL_TWINKLES_MODE_STRING          "* FL Twinkles"
