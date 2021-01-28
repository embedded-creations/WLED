#pragma once

// This example effect's name is "BLINK_TRIGGERS" - `#define USERFX1 BLINK_TRIGGERS` will assign this BLINK_TRIGGERS effect to UserFX slot number 1

// https://github.com/marmilicious/FastLED_examples/blob/master/every_n_timer_variable.ino
uint16_t mode_blinktriggers(void) {
  FL_ALLOC(
    boolean counterTriggered = 0;  // Event triggered? [1=true, 0=false]
    CEveryNMilliseconds triggerTimer;
    CEveryNMillis flMillisTrigger1;
    CEveryNSeconds timingObj;
  );

  int msUntilNextPass = FRAMETIME;

  const uint16_t timerA = 3000;  // How often to run Event A [milliseconds]
  const uint16_t timerB = 500;  // How long after A to run Event B [milliseconds]

  // This runs once when the effect starts, similar to setup() in a FastLED sketch
  FL_SETUP() {
    FL_STATICVAR(counterTriggered) = 0;

    // make a macro?  These calls replace static CEveryNMilliseconds triggerTimer(timerB);
    FL_STATICVAR(triggerTimer).reset();
    FL_STATICVAR(triggerTimer).setPeriod(timerB);
  }

  FL_EVERY_N_MILLIS(flMillisTrigger1, timerA) {
    // do Event A stuff
    fill_solid(leds, NUM_LEDS, CHSV(random8(),255,128));
    FL_STATICVAR(counterTriggered) = 1;  // Set to True
    FL_STATICVAR(triggerTimer).reset();  // Start trigger timer
  }

  if (FL_STATICVAR(counterTriggered) == 1) {  // Will only be True if Event A has started
    if (FL_STATICVAR(triggerTimer)) {  // Check if triggerTimer time reached
      // do Event B stuff
      for (uint8_t i=0; i<NUM_LEDS/2; i++){
        leds[random16(NUM_LEDS)] = CRGB::Red;
      }
      FL_STATICVAR(counterTriggered) = 0;  // Set back to False
    }
  }

#if 0
  // this block doesn't translate easily to WLED as it uses delays in a loop and nested FastLED.show() calls
  FL_EVERY_N_SECONDS(timingObj, 20) {
    // This initally defaults to 20 seconds, but then will change the run
    // period to a new random number of seconds from 10 and 30 seconds.
    // You can name "timingObj" whatever you want.
    FL_STATICVAR(timingObj).setPeriod( random8(10,31) );
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    for (uint16_t i=0; i<NUM_LEDS*3; i++){
      leds[random8(NUM_LEDS)] = CRGB::Black;
      leds[random8(NUM_LEDS)] = CHSV(random8(), random8(140,255), random8(50,255));
      FastLED.show();
      delay(random8(20,80));
    }
    msUntilNextPass = 500
  }
#endif

  END_FASTLED_COMPATIBILITY();

  return msUntilNextPass;  
}

// set this to the name of your effect matching the name of the function defined above
#define BLINK_TRIGGERS_MODE_NAME            mode_blinktriggers

// set this to the name of your effect as you want it shown in the web interface
#define BLINK_TRIGGERS_MODE_STRING          "* Blink triggers"
