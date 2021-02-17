#include "wled.h"
/*
 * Register your v2 usermods here!
 *   (for v1 usermods using just usermod.cpp, you can ignore this file)
 */

/*
 * Add/uncomment your usermod filename here (and once more below)
 * || || ||
 * \/ \/ \/
 */
//#include "usermod_v2_example.h"
#ifdef USERMOD_DALLASTEMPERATURE
#include "../usermods/Temperature/usermod_temperature.h"
#endif

//#include "usermod_v2_empty.h"

#ifdef USERMOD_BUZZER
#include "../usermods/buzzer/usermod_v2_buzzer.h"
#endif
#ifdef USERMOD_SENSORSTOMQTT
#include "usermod_v2_SensorsToMqtt.h"
#endif

#if (defined(ENABLE_FL_MAPPING) || defined(ENABLE_FL_WRAPPER))
  #include "../usermods/FastLED_Compatibility/FL_Mapping_Usermod.h"
#endif
#ifdef USERMOD_ANIMATEDGIFS
  #include <SD.h> // needed here to make it onto PlatformIO Dependency Graph, otherwise would only need it inside the Usermod
  #include "../usermods/AnimatedGIFs/usermod_animatedgifs.h"
#endif


// BME280 v2 usermod. Define "USERMOD_BME280" in my_config.h
#ifdef USERMOD_BME280
#include "../usermods/BME280_v2/usermod_bme280.h"
#endif
#ifdef USERMOD_FOUR_LINE_DISLAY
#include "../usermods/usermod_v2_four_line_display/usermod_v2_four_line_display.h"
#endif
#ifdef USERMOD_ROTARY_ENCODER_UI
#include "../usermods/usermod_v2_rotary_encoder_ui/usermod_v2_rotary_encoder_ui.h"
#endif
#ifdef USERMOD_AUTO_SAVE
#include "../usermods/usermod_v2_auto_save/usermod_v2_auto_save.h"
#endif

#ifdef USERMOD_DHT
#include "../usermods/DHT/usermod_dht.h"
#endif

void registerUsermods()
{
/*
   * Add your usermod class name here
   * || || ||
   * \/ \/ \/
   */
  //usermods.add(new MyExampleUsermod());
  
  #ifdef USERMOD_DALLASTEMPERATURE
  usermods.add(new UsermodTemperature());
  #endif
  
  //usermods.add(new UsermodRenameMe());
  
  #ifdef USERMOD_BUZZER
  usermods.add(new BuzzerUsermod());
  #endif
  
  #ifdef USERMOD_BME280
  usermods.add(new UsermodBME280());
  #endif

  #ifdef USERMOD_SENSORSTOMQTT
  usermods.add(new UserMod_SensorsToMQTT());
  #endif

  #ifdef USERMOD_ROTARY_ENCODER_UI
  usermods.add(new RotaryEncoderUIUsermod());
  #endif

  #ifdef USERMOD_DHT
  usermods.add(new UsermodDHT());
  #endif

  #ifdef ENABLE_FL_MAPPING
  usermods.add(new Usermod_FL_Mapping());
  #endif

  #ifdef USERMOD_ANIMATEDGIFS
  usermods.add(new AnimatedGifsUsermod());
  #endif
}

