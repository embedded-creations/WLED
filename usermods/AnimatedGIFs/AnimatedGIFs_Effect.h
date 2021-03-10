#pragma once

// This example effect's name is "AGIFS_TEST" - `#define USERFX1 AGIFS_TEST` will assign this AGIFS_TEST effect to UserFX slot number 1

// TODO: include file with function definition
//CRGB * agifsGetVirtualScreen(uint16_t width, uint16_t height);

uint16_t mode_agifs2D(void) {
  FL_GET_VIRTUAL_SCREEN(agifsGetVirtualScreen);      // This effect depends on leds[] being sized to kMatrixWidth * kMatrixHeight, and uses XY()

  FL_ALLOC(
    uint8_t intensityPrev;
    );

  // if the intensity slider changed, tell the animatedGIFs Usermod to switch to the new index on the next frame
  if(FL_STATICVAR(intensityPrev) != SEGMENT.intensity) {
    FL_STATICVAR(intensityPrev) = SEGMENT.intensity;
    agifsSetNextGifIndex(SEGMENT.intensity, true);
  }

  END_FASTLED_COMPATIBILITY(); // this macro writes the contents of leds[] to WLED

  // we assume we're in the middle (ideally towards the beginning) of when the current frame was decoded, so delaying by the current frameDelay should put us in the same position on the next frame. (Not true if we can't keep up with the current GIF frame rate)
  uint16_t delay = agifsGetCurrentFrameDelay();

  #if 0
    // TODO: if there's not much time between nextFrameTime and now+delay, then we may end up skipping frames or having jitter as we show either the expected frame or the previous frame depending on what variable delays are going on in the system.  Reduce the delay to give more of a gap until we catch up?
    uint32_t nextFrameTime = agifsGetNextFrameDisplayTime();
    uint32_t nextShowTime = now - timebase;
    int32_t wiggleroom = (int32_t)nextFrameTime - ((int32_t)nextShowTime + delay); // expected values: negative, possibly exceeding delay, do we need to subtract delay to do this math?

    // TODO: fine tune this value, maybe make it based on delay? so a delay of 10 can be handled as well as 20+?
    if(abs(wiggleroom) < 5)
      delay--; // maybe this should be ++ instead?
    Serial.print(wiggleroom);
    Serial.print(",");
    Serial.print(delay);
    Serial.print(",");
  #endif

  return delay;
}

// set this to the name of your effect matching the name of the function defined above
#define AGIFS_TEST_MODE_NAME            mode_agifs2D

// set this to the name of your effect as you want it shown in the web interface
#define AGIFS_TEST_MODE_STRING          "*2D AnimatedGIFs"
