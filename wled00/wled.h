#ifndef WLED_H
#define WLED_H

//#define WLED_USE_MY_CONFIG

#include <Arduino.h>

#ifdef WLED_USE_MY_CONFIG
  #include "my_config.h"
#endif

#include "FX.h"

extern WS2812FX strip;

#endif
