#ifndef WLED_H
#define WLED_H

//uncomment this if you have a "my_config.h" file you'd like to use
//#define WLED_USE_MY_CONFIG

#include <Arduino.h>

#ifdef WLED_USE_MY_CONFIG
  #include "my_config.h"
#endif

#include "FX.h"

extern WS2812FX strip;

#endif
