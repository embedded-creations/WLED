//this code is a modified version of https://github.com/Makuna/NeoPixelBus/issues/103
#ifndef NpbWrapper_h
#define NpbWrapper_h

//PIN CONFIGURATION
#ifndef LEDPIN
#define LEDPIN 2  //strip pin. Any for ESP32, gpio2 or 3 is recommended for ESP8266 (gpio2/3 are labeled D4/RX on NodeMCU and Wemos)
#endif
//#define USE_APA102  // Uncomment for using APA102 LEDs.
//#define USE_WS2801  // Uncomment for using WS2801 LEDs (make sure you have NeoPixelBus v2.5.6 or newer)
//#define USE_LPD8806 // Uncomment for using LPD8806
//#define USE_TM1814  // Uncomment for using TM1814 LEDs (make sure you have NeoPixelBus v2.5.7 or newer)
//#define USE_P9813   // Uncomment for using P9813 LEDs (make sure you have NeoPixelBus v2.5.8 or newer)
//#define WLED_USE_ANALOG_LEDS //Uncomment for using "dumb" PWM controlled LEDs (see pins below, default R: gpio5, G: 12, B: 15, W: 13)
//#define WLED_USE_H801 //H801 controller. Please uncomment #define WLED_USE_ANALOG_LEDS as well
//#define WLED_USE_5CH_LEDS  //5 Channel H801 for cold and warm white
//#define WLED_USE_BWLT11
//#define WLED_USE_SHOJO_PCB

#ifndef BTNPIN
#define BTNPIN  0  //button pin. Needs to have pullup (gpio0 recommended)
#endif

#ifndef TOUCHPIN
//#define TOUCHPIN T0 //touch pin. Behaves the same as button. ESP32 only.
#endif

#ifndef IRPIN
#define IRPIN  4  //infrared pin (-1 to disable)  MagicHome: 4, H801 Wifi: 0
#endif

#ifndef RLYPIN
#define RLYPIN 12  //pin for relay, will be set HIGH if LEDs are on (-1 to disable). Also usable for standby leds, triggers,...
#endif

#ifndef AUXPIN
#define AUXPIN -1  //debug auxiliary output pin (-1 to disable)
#endif

#ifndef RLYMDE
#define RLYMDE  1  //mode for relay, 0: LOW if LEDs are on 1: HIGH if LEDs are on
#endif

//enable color order override for a specific range of the strip
//This can be useful if you want to chain multiple strings with incompatible color order
//#define COLOR_ORDER_OVERRIDE
#define COO_MIN    0
#define COO_MAX   35 //not inclusive, this would set the override for LEDs 0-34
#define COO_ORDER COL_ORDER_GRB

//END CONFIGURATION

#if defined(USE_APA102) || defined(USE_WS2801) || defined(USE_LPD8806) || defined(USE_P9813)
 #ifndef CLKPIN
  #define CLKPIN 0
 #endif
 #ifndef DATAPIN
  #define DATAPIN 2
 #endif
 #if BTNPIN == CLKPIN || BTNPIN == DATAPIN
  #undef BTNPIN   // Deactivate button pin if it conflicts with one of the APA102 pins.
 #endif
#endif

#ifdef WLED_USE_ANALOG_LEDS
  //PWM pins - PINs 15,13,12,14 (W2 = 04)are used with H801 Wifi LED Controller
  #ifdef WLED_USE_H801
    #define RPIN 15   //R pin for analog LED strip   
    #define GPIN 13   //G pin for analog LED strip
    #define BPIN 12   //B pin for analog LED strip
    #define WPIN 14   //W pin for analog LED strip 
    #define W2PIN 04  //W2 pin for analog LED strip
    #undef BTNPIN
    #undef IRPIN
    #define IRPIN  0 //infrared pin (-1 to disable)  MagicHome: 4, H801 Wifi: 0
  #elif defined(WLED_USE_BWLT11)
  //PWM pins - to use with BW-LT11
    #define RPIN 12  //R pin for analog LED strip
    #define GPIN 4   //G pin for analog LED strip
    #define BPIN 14  //B pin for analog LED strip
    #define WPIN 5   //W pin for analog LED strip
  #elif defined(WLED_USE_SHOJO_PCB)
  //PWM pins - to use with Shojo PCB (https://www.bastelbunker.de/esp-rgbww-wifi-led-controller-vbs-edition/)
    #define RPIN 14  //R pin for analog LED strip
    #define GPIN 4   //G pin for analog LED strip
    #define BPIN 5   //B pin for analog LED strip
    #define WPIN 15  //W pin for analog LED strip
    #define W2PIN 12 //W2 pin for analog LED strip
  #elif defined(WLED_USE_PLJAKOBS_PCB)
  // PWM pins - to use with esp_rgbww_controller from patrickjahns/pljakobs (https://github.com/pljakobs/esp_rgbww_controller)
    #define RPIN 12  //R pin for analog LED strip
    #define GPIN 13  //G pin for analog LED strip
    #define BPIN 14  //B pin for analog LED strip
    #define WPIN 4   //W pin for analog LED strip
    #define W2PIN 5  //W2 pin for analog LED strip
    #undef IRPIN
  #else
  //Enable override of Pins by using the platformio_override.ini file
  //PWM pins - PINs 5,12,13,15 are used with Magic Home LED Controller
    #ifndef RPIN
      #define RPIN 5   //R pin for analog LED strip
    #endif
    #ifndef GPIN
      #define GPIN 12  //G pin for analog LED strip
    #endif
    #ifndef BPIN
      #define BPIN 15  //B pin for analog LED strip
    #endif
    #ifndef WPIN
      #define WPIN 13  //W pin for analog LED strip
    #endif
  #endif
  #undef RLYPIN
  #define RLYPIN -1 //disable as pin 12 is used by analog LEDs
#endif

//automatically uses the right driver method for each platform
#ifdef ARDUINO_ARCH_ESP32
 #ifdef USE_APA102
  #define PIXELMETHOD DotStarMethod
 #elif defined(USE_WS2801)
  #define PIXELMETHOD NeoWs2801Method
 #elif defined(USE_LPD8806)
  #define PIXELMETHOD Lpd8806Method
 #elif defined(USE_TM1814)
  #define PIXELMETHOD NeoTm1814Method  
 #elif defined(USE_P9813)
  #define PIXELMETHOD P9813Method  
 #else
  #define PIXELMETHOD NeoEsp32Rmt0Ws2812xMethod
 #endif
#else //esp8266
 //autoselect the right method depending on strip pin
 #ifdef USE_APA102
  #define PIXELMETHOD DotStarMethod
 #elif defined(USE_WS2801)
  #define PIXELMETHOD NeoWs2801Method
 #elif defined(USE_LPD8806)
  #define PIXELMETHOD Lpd8806Method
 #elif defined(USE_TM1814)
  #define PIXELMETHOD NeoTm1814Method  
 #elif defined(USE_P9813)
  #define PIXELMETHOD P9813Method  
 #elif LEDPIN == 2
  #define PIXELMETHOD NeoEsp8266Uart1Ws2813Method //if you get an error here, try to change to NeoEsp8266UartWs2813Method or update Neopixelbus
 #elif LEDPIN == 3
  #define PIXELMETHOD NeoEsp8266Dma800KbpsMethod
 #else
  #define PIXELMETHOD NeoEsp8266BitBang800KbpsMethod
  #pragma message "Software BitBang will be used because of your selected LED pin. This may cause flicker. Use GPIO 2 or 3 for best results."
 #endif
#endif


//you can now change the color order in the web settings
#ifdef USE_APA102
 #define PIXELFEATURE3 DotStarBgrFeature
 #define PIXELFEATURE4 DotStarLbgrFeature
#elif defined(USE_LPD8806)
 #define PIXELFEATURE3 Lpd8806GrbFeature 
 #define PIXELFEATURE4 Lpd8806GrbFeature 
#elif defined(USE_WS2801)
 #define PIXELFEATURE3 NeoRbgFeature
 #define PIXELFEATURE4 NeoRbgFeature
#elif defined(USE_TM1814)
  #define PIXELFEATURE3 NeoWrgbTm1814Feature
  #define PIXELFEATURE4 NeoWrgbTm1814Feature
#elif defined(USE_P9813)
 #define PIXELFEATURE3 P9813BgrFeature 
 #define PIXELFEATURE4 NeoGrbwFeature   
#else
 #define PIXELFEATURE3 NeoGrbFeature
 #define PIXELFEATURE4 NeoGrbwFeature
#endif

struct RgbwColor
{
    RgbwColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0) :
        R(r), G(g), B(b), W(w)
    {
    };

    RgbwColor(uint8_t brightness) :
        R(0), G(0), B(0), W(brightness)
    {
    };

    RgbwColor()
    {
    };

    bool operator==(const RgbwColor& other) const
    {
        return (R == other.R && G == other.G && B == other.B && W == other.W);
    };

    bool operator!=(const RgbwColor& other) const
    {
        return !(*this == other);
    };

    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t W;
};

#include "const.h"

enum NeoPixelType
{
  NeoPixelType_None = 0,
  NeoPixelType_Grb  = 1,
  NeoPixelType_Grbw = 2,
  NeoPixelType_End  = 3
};

class NeoPixelWrapper
{
public:
  NeoPixelWrapper() :
    // initialize each member to null
    _type(NeoPixelType_None)
  {
    // TODO: init FastLED based on the pixel type set in configuration
    FastLED.addLeds<NEOPIXEL, LEDPIN>(FL_leds, MAX_LEDS);  // GRB ordering is assumed
  }

  ~NeoPixelWrapper()
  {
    cleanup();
  }

  void Begin(NeoPixelType type, uint16_t countPixels)
  {
    cleanup();
    _type = type;

  }

  void Show()
  {
    FastLED.show();
  }

  /** 
   * This will return true if enough time has passed since the last time Show() was called. 
   * This also means that calling Show() will not cause any undue waiting. If the method for 
   * the defined bus is hardware that sends asynchronously, then call CanShow() will let 
   * you know if it has finished sending the data from the last Show().
   */
  bool CanShow()
  {
    return true;
  }

  void SetPixelColor(uint16_t indexPixel, RgbwColor c)
  {
    CRGB fastled_col;
    fastled_col.red =   c.R;
    fastled_col.green = c.G;
    fastled_col.blue =  c.B;
    FL_leds[indexPixel] = fastled_col;
  }

  void SetBrightness(byte b)
  {
    FastLED.setBrightness(b);
  }

  void SetColorOrder(byte colorOrder) {
    _colorOrder = colorOrder;
  }

  uint8_t GetColorOrder() {
    return _colorOrder;
  }

  RgbwColor GetPixelColorRaw(uint16_t indexPixel) const
  {
    RgbwColor retColor;
    retColor.R = FL_leds[indexPixel].red;
    retColor.B = FL_leds[indexPixel].green;
    retColor.G = FL_leds[indexPixel].blue;
    return 0;
  }

  // NOTE:  Due to feature differences, some support RGBW but the method name
  // here needs to be unique, thus GetPixeColorRgbw
  uint32_t GetPixelColorRgbw(uint16_t indexPixel) const
  {
    uint32_t retColor;
    retColor = FL_leds[indexPixel].red;
    retColor |= FL_leds[indexPixel].green << 8;
    retColor |= FL_leds[indexPixel].blue << 16;
    return retColor;
  }

  uint8_t* GetPixels(void)
  {
    return 0;
  }

private:
  NeoPixelType _type;

  byte _colorOrder = 0;

  CRGB FL_leds[MAX_LEDS];

  void cleanup()
  {

  }
};
#endif
