# FastLED Compatibility UserFX Wrapper

The FastLED Compatibility UserFX Wrapper (FL_Wrapper) allows for easier porting of FastLED effects to WLED.

## Functionality

FastLED has some great features that are taken for granted when writing FastLED patterns, but can't be used directly when writing WLED effects.  FL_Wrapper brings these features into WLED to make writing effects and porting FastLED effects into WLED much easier.

### Persistent Non-Lossy CRGB Storage

FastLED sketches usually store pixel data in `CRGB leds[]`, and define `NUM_LEDS` at the top of the sketch.  `leds[]` is also persistent between passes through `loop()`, so effects can be made by the data between previous frames.  This won't work with WLED, as `NUM_LEDS` isn't a constant, it's determined by the settings of the current segment.  Likewise, a fixed `leds[]` array wouldn't work as there are multiple segments that would need to access it.  Also, WLED uses `setPixelColor()` to store RGB data instead of directly accessing an array, and that data can't be directly retrieved like you would from an array, as `getPixelColor()` returns lossy data.

To make it easier to port FastLED sketches, FL_Wrapper manages an array that can be used for persistent storage of CRGB data, that can be accessed using `leds[]` in your effect, and sets a local constant `NUM_LEDS`.  For this basic functionality, use `BEGIN_FASTLED_COMPATIBILITY()` at the top of the effect.  At the end of the effect, use `END_FASTLED_COMPATIBILITY()`, which takes care of writing the `leds[]` array to WLED's buffers using `setPixelColor()`.  The more complex macros describe later also include this feature.

### 2D Functionality ("Virtual Screen")

Some FastLED sketches are made for rectangular matrices, where the width/height is set to `kMatrixWidth`/`kMatrixHeight`, `NUM_LEDS` is `kMatrixWidth*kMatrixHeight`, and an `XY()` function is defined that can be used in the sketch to set the LED matching an X/Y coordinate in the matrix: `leds[XY(x,y)]`.

The `BEGIN_FASTLED_XY_COMPATIBILITY()` macro sets up all the FastLED compatibility as `BEGIN_FASTLED_COMPATIBILITY()` does, and also creates these variables:

```
kMatrixWidth = _segmentmaps[_segment_index].matrixWidth;
kMatrixHeight = _segmentmaps[_segment_index].matrixHeight;
int NUM_LEDS (kMatrixWidth*kMatrixHeight);
```

The `XY()` function is created and depends on the above `kMatrixWidth` and `kMatrixHeight` values.  It is accessible to FastLED's `blur2d()` function.

The `END_FASTLED_XY_COMPATIBILITY()` macro writes the contents of the `leds[]` array to WLED's pixel buffer

### Allocating Persistent Memory for Variables

FastLED sketches often store data in global variables inside the sketch, or static variables inside a function like `loop()`, where the value will remain the same between calls to the function.  This doesn't work with WLED, as having dedicated memory for an effect that's not running won't scale when there's many effects to choose from.  Also, each effect could be running on multiple segments, where each variable might have different data.

WLED allows data to be allocated specific segment when the effect first runs, but it's cumbersome to use for more than one object, and we're already allocating an object for the `leds[]` array.  `FL_ALLOC()` simplifies this by allocating memory for both the `leds[]` array and multiple variables.

For example to allocate memory for `unsigned long prevMillis` use this at the top of the sketch:

```
  FL_ALLOC(
    unsigned long prevMillis;
    );
```

This does everything `BEGIN_FASTLED_COMPATIBILITY()` does, plus allocates memory for the variables described between the parentheses.  You can allocate memory for multiple variables.

The variables are allocated in a struct, and can't be used directly.  To use one of the variables, wrap it in `FL_STATICVAR()`, e.g.

```
unsigned long curMillis = millis();
if ((curMillis - FL_STATICVAR(prevMillis)) >= 100) {
  FL_STATICVAR(prevMillis) = curMillis;
}
```

Use `END_FASTLED_COMPATIBILITY()` at the end of the effect.

TODO: create macro for `FL_ALLOC_XY()`, in the meantime use `FL_ALLOC_WITH_1_ARRAY_XY()` and create a dummy array to store 1 byte.

### Allocating Persistent Memory for Arrays

Arrays need to be allocated differently than variables, as they can't be put directly into the struct if they depend on a parameter that is variable per segment, e.g. `NUM_LEDS`.  The macro `FL_ALLOC_WITH_1_ARRAY_XY()` can allocate variables and an array. To allocate memory for `byte heat[NUM_LEDS]`, use:

```
  FL_ALLOC_WITH_1_ARRAY_XY(
    unsigned long prevMillis;
    , byte, heat, NUM_LEDS
    );
```

Note you can leave the first argument of the macro (where you'd put one or more variables to allocate) empty if you only need an array and no persistent variables.

Use the array just like you would a persistent variable, by wrapping it in `FL_STATICVAR()` e.g.

```
for (int mh = 0; mh < kMatrixHeight; mh++) {
  FL_STATICVAR(heat)[mw*kMatrixWidth+mh] = qsub8( FL_STATICVAR(heat)[mw*kMatrixWidth+mh],  random16(0, ((COOLING * 10) / kMatrixHeight) + 2));
}
```

TODO: create macro for `FL_ALLOC_WITH_1_ARRAY()` (no XY) and options for multiple arrays

### Setup()

If your FastLED sketch needs to run some code once during `Setup()`, add it to `FL_SETUP() { }` e.g.

```
FL_SETUP() {
  FL_STATICVAR(counterTriggered) = 0;
}
```

### Triggers (e.g. `EVERY_N_MILLIS()`)

FastLED has a really handy feature called triggers which allows code to be run periodically without having to keep track of the passage of time manually.  This doesn't work with WLED as is because behind the scenes, the trigger macro creates a static variable to hold a trigger timer object.  We can create the trigger timer object manually as we normally do for static variables, and then call it with a modified macro: `FL_EVERY_N()`.  To create a milliseconds trigger set to trigger every 100ms and use it:

```
FL_ALLOC(
  CEveryNMillis flMillisTrigger1;
);

FL_EVERY_N_MILLIS(flMillisTrigger1, 100) {
	// do something every 100ms here
}
```

These macros are defined for use with triggers:

```
FL_EVERY_N_MILLIS()
FL_EVERY_N_SECONDS()
FL_EVERY_N_BSECONDS()
FL_EVERY_N_MINUTES()
FL_EVERY_N_HOURS()
```

If you want to do more advanced things with the trigger timer, use it as you would another static variable. e.g.

```
FL_STATICVAR(triggerTimer).setPeriod(200);
``` 

### Nested Functions

Often a FastLED sketch will use multiple functions to simplify the code, but this isn't easy to port into WLED.  Some of the FL_Wrapper features described above use local variables which won't be accessible from another function.  The `FL_FUNCTIONS_BEGIN`, `FL_FUNCTIONS_END`, and `FL_FUNCTION()` macros allow for nesting functions and calling function inside the WLED effect.  An example showing two nested functions:

```
FL_FUNCTIONS_BEGIN // vvvvvvvvv Local Functions go here
    void brightenOrDarkenEachPixel( fract8 fadeUpAmount, fract8 fadeDownAmount)
    {
     for( uint16_t i = 0; i < NUM_LEDS; i++) {
        if( getPixelDirection(i) == GETTING_DARKER) {
          // This pixel is getting darker
          leds[i] = makeDarker( leds[i], fadeDownAmount);
        }
    }
    bool getPixelDirection( uint16_t i) {
      uint16_t index = i / 8;
      uint8_t  bitNum = i & 0x07;
      return bitRead( FL_STATICVAR(directionFlags)[index], bitNum);
    }
FL_FUNCTIONS_END // ^^^^^^^^^ Local Functions go here

FL_FUNCTION(brightenOrDarkenEachPixel)( FADE_IN_SPEED, FADE_OUT_SPEED);
```

Note that to call a nested function from the main effect function, you need to wrap the function name in `FL_FUNCTION()`.  To call a nested function from another nested function, you can't wrap the function name in `FL_FUNCTION()`.  Nested functions need to wrap static variables/arrays in `FL_STATICVAR()`

TODO: make it so `FL_FUNCTION()` use is consistent across the main and nested functions?

## Usage

Define `ENABLE_FL_WRAPPER`.  It's not useful as is without UserFX, so define `ENABLE_USERFX` at the same time.  Add to myconfig.h:

```
#define ENABLE_USERFX
#define ENABLE_FL_WRAPPER
```

or Add to PlatformIO Config:

```
-D ENABLE_USERFX
-D ENABLE_FL_WRAPPER
```

Now add one or more effects using FL_Wrapper, each one in its own UserFX slot.  To include the SWIRL_2D example in UserFX slot 1, add to myconfig.h:

```
#define USERFX1_H "../usermods/FastLED_Compatibility/FL_Example_Swirl2D.h"
#define USERFX1_NAME SWIRL2D
```

or Add to PlatformIO Config:

```
'-D USERFX1_H="../usermods/FastLED_Compatibility/FL_Example_Swirl2D.h"'
-D USERFX1_NAME=SWIRL2D
```

Compile and Upload and you should see a new "*2D Swirl" effect at the top of the Effects list.

For 2D effects, edit `FL_Wrapper.cpp.h`, changing the values and entries in `_segmentmaps[]` to match your LEDs.  Note this table will hopefully be integrated into the main `_segments[]` table with a web interface in the future.

TODO: describe how to fill in the table in detail

## Improvements

This UserFX concept is just a prototype, and the API will likely change in the future as the prototyping continues

Note some of the "TODO"s above marking incomplete features

