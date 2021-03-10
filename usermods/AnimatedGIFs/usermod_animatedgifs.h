#pragma once

#include "wled.h"

#include <LITTLEFS.h>
#include <SD.h>
#include <GifDecoder.h>

#include "animatedgifs_common.h"
#include "FilenameFunctions.cpp"

// these example GIFs are from the AnimatedGIF library, see that library for the source, and instructions on how to make your own GIF into a C header file
#include "homer_tiny.h"
#include "badgers.h"

// these gifs were made by resizing the original GIFs (source unknown) using ezgif.com and converted to C headers using https://github.com/bitbank2/image_to_c
#include "infinity1.h"
//#include "infinity2.h"
#include "rainbow1.h"
#include "rainbow2.h"

// add gif name and sizeof(gif) to the below lists
const uint8_t * gifsList[] = { rainbow2, rainbow1, infinity1 };
const char * gifsNameList[] = { "rainbow2", "rainbow1", "infinity1" }; // can contain zero-length or NULL strings, or a zero-length array if you don't want to print the name)
const int gifsSizeList[] = { sizeof(rainbow2), sizeof(rainbow1), sizeof(infinity1) };
//const uint8_t * gifsList[] = { infinity1 };
//const char * gifsNameList[] = { "infinity1" }; // can contain zero-length or NULL strings, or a zero-length array if you don't want to print the name)
//const int gifsSizeList[] = { sizeof(infinity1) };

// defaults in case config hasn't been written to cfg.json yet
#define DISPLAY_TIME_SECONDS 10
#define NUMBER_FULL_CYCLES   100
#define AGIF_MATRIX_WIDTH 64
#define AGIF_MATRIX_HEIGHT 64
#define PLAYBACK_MODE playModeFirst

// SD card support isn't enabled by default, define here or in my_config/PlatformIO config
//#define AGIFS_USE_SD

// The ESP32 SD library has a bug that sometimes requires SD.begin() to be called multiple times to work, increase this if needed
#define SD_NUM_TRIES 2


// ESP32 FS Libraries can't handle a trailing slash in the directory name
#define GIF_DIRECTORY_SD "/gifs"
#define GIF_DIRECTORY_LITTLEFS "/"

int num_files_SD = 0;
int num_files_LittleFS = 0;
const int num_files_Memory = sizeof(gifsList) ? sizeof(gifsList)/sizeof(gifsList[0]) : 0;


bool useLittleFS = true;
#ifdef AGIFS_USE_SD
  bool useSD = true;
#else
  bool useSD = false;
#endif

int gifMatrixWidth;
int gifMatrixHeight;
int displayTimeSeconds;
int numFullCycles;
int playMode;
bool playNextGif = true;     // we haven't loaded a GIF yet on first pass through, make sure we do that
int gifIndex = 0;
int agifSdPinClk = -1;
int agifSdPinMiso = -1;
int agifSdPinMosi = -1;
int agifSdPinCs = -1;

// decoder has width=320 (max AnimatedGIF library can support), height=lzwMax=0 (doesn't matter), use malloc for buffer
GifDecoder<320, 0, 12, true> decoder;

CRGB * gifMatrixBuffer;

void screenClearCallback(void) {
  fill_solid(gifMatrixBuffer, gifMatrixWidth*gifMatrixHeight, CRGB::Black);
}

void updateScreenCallback(void) {
  // don't need to do anything here
}

void drawPixelCallback(int16_t x, int16_t y, uint8_t red, uint8_t green, uint8_t blue) {
  if(x < gifMatrixWidth && y < gifMatrixHeight)
    gifMatrixBuffer[(y*gifMatrixWidth) + x] = CRGB(red, green, blue);
}

void * agifsGetVirtualScreen(uint16_t width, uint16_t height) {
  kMatrixWidth = gifMatrixWidth;
  kMatrixHeight = gifMatrixHeight;

  return (void*)gifMatrixBuffer;
}

void agifsSetNextGifIndex(int index, bool switchImmediately) {
  gifIndex = index;
  if(switchImmediately)
    playNextGif = true;
}

unsigned int currentFrameDelay = 0;
uint32_t lastFrameDisplayTime = 0;

uint16_t agifsGetCurrentFrameDelay() {
  return currentFrameDelay;
}

uint32_t agifsGetNextFrameDisplayTime() {
  return lastFrameDisplayTime + currentFrameDelay;
}

uint16_t agifsGetMillisToNextFrameDisplay() {
  uint32_t now = millis();

  if (((lastFrameDisplayTime + currentFrameDelay) - now) & 0x80000000) { // test the "sign" bit - if true, then next frame display time is in the past
    return 0;
  } else {
    return (lastFrameDisplayTime + currentFrameDelay) - now;
  }
}

#ifdef AGIFS_USE_SD
  int initFileSystem_SD() {
    bool result;
    int tries = 1;

    // don't use SD if the critical pins aren't configured (sdCs is optional)
    if((agifSdPinClk < 0) || (agifSdPinMiso < 0) || (agifSdPinMosi < 0))
      return -1;

    SPI.begin(agifSdPinClk, agifSdPinMiso, agifSdPinMosi, agifSdPinCs); // normal SPI with CS

    while(!(result = SD.begin(agifSdPinCs, SPI, 10000000)))
    {
      Serial.print("fail");
      delay(10);
      if(tries >= SD_NUM_TRIES)
        break;
      tries++;
    }

    if(!result) {
      Serial.print("Card Mount Failed after ");
      Serial.print(tries);
      Serial.println(" tries");
      return -1;
    } else {
      Serial.print("SD Card Mounted after ");
      Serial.print(tries);
      Serial.println(" tries");
      return 0;
    }
  }
#endif

int initFileSystem_LITTLEFS() {
  if (!LITTLEFS.begin())
    return -1;
  else
    return 0;
}

class AnimatedGifsUsermod : public Usermod {
  public:
    void setup() {
      gifMatrixBuffer = (CRGB *)malloc(sizeof(CRGB) * (gifMatrixWidth * gifMatrixHeight));

      decoder.setScreenClearCallback(screenClearCallback);
      decoder.setUpdateScreenCallback(updateScreenCallback);
      decoder.setDrawPixelCallback(drawPixelCallback);

      decoder.setFileSeekCallback(fileSeekCallback);
      decoder.setFilePositionCallback(filePositionCallback);
      decoder.setFileReadCallback(fileReadCallback);
      decoder.setFileReadBlockCallback(fileReadBlockCallback);
      decoder.setFileSizeCallback(fileSizeCallback);

      Serial.println("AnimatedGIFs Usermod:");

      for(int i=0; i<num_files_Memory; i++) {
        Serial.print(i);
        Serial.print(" - Memory:");
        if(i < sizeof(gifsNameList) && gifsNameList[i] && strlen(gifsNameList[i])) {
          Serial.println(gifsNameList[i]);
        } else {
          Serial.print("gifsList[");
          Serial.print(i);
          Serial.println("]");
        }
      }

      if(initFileSystem_LITTLEFS() < 0) {
        Serial.println("No LITTLEFS");
        useLittleFS = false;
      } else {
        // Determine how many animated GIF files exist in LittleFS
        num_files_LittleFS = enumerateGIFFiles(LITTLEFS, GIF_DIRECTORY_LITTLEFS, true, "LittleFS", num_files_Memory);      
      }

      if(useLittleFS && num_files_LittleFS < 0) {
        Serial.println("No LittleFS gifs directory");
        useLittleFS = false;
        num_files_LittleFS = 0;
      }

      if(useLittleFS && !num_files_LittleFS) {
        Serial.println("Empty LittleFS gifs directory");
        useLittleFS = false;
      }    

      #ifdef AGIFS_USE_SD
        if(initFileSystem_SD() < 0) {
          Serial.println("No SD card");
          useSD = false;
        } else {
          // Determine how many animated GIF files exist in SD card
          num_files_SD = enumerateGIFFiles(SD, GIF_DIRECTORY_SD, true, "SD", num_files_Memory + num_files_LittleFS);      
        }

        if(useSD && num_files_SD < 0) {
          Serial.println("No SD gifs directory");
          useSD = false;
          num_files_SD = 0;
        }

        if(useSD && !num_files_SD) {
          Serial.println("Empty SD gifs directory");
        }
      #endif

      #ifdef AGIFS_USE_SD
        Serial.print("useSD: ");
        Serial.println(useSD);
        Serial.print("num_files_SD: ");
        Serial.println(num_files_SD);
      #endif

      Serial.print("useLittleFS: ");
      Serial.println(useLittleFS);
      Serial.print("num_files_LittleFS: ");
      Serial.println(num_files_LittleFS);

      Serial.print("num_files_Memory: ");
      Serial.println(num_files_Memory);

      if(!num_files_SD && !num_files_LittleFS && !num_files_Memory) {
        Serial.println("No GIFs to play");
      }
    }

    void loop() {
      if(!configExists) {
        Serial.println("Agifs: Need to save config");
        configExists = true;
        serializeConfig();
      }

      // these variables keep track of when it's time to play a new GIF
      static unsigned long displayStartTime_millis;
      

      unsigned long now = millis();

      // just return here if there's no GIFs to play
      if(!num_files_SD && !num_files_LittleFS && !num_files_Memory)
        return;

      // default behavior is to play the gif for displayTimeSeconds or for numFullCycles, whichever comes first
      if((playMode == playModeFirst) && ((now - displayStartTime_millis) > (displayTimeSeconds * 1000) || decoder.getCycleNumber() > numFullCycles)) {
        playNextGif = true;

      }
      // alt behavior is to play the gif until both displayTimeSeconds and numFullCycles have passed
      else if((playMode == playModeBoth) && ((now - displayStartTime_millis) > (displayTimeSeconds * 1000) && decoder.getCycleNumber() > numFullCycles)) {
        playNextGif = true;  
      }

      // We only decode a GIF frame if the previous frame delay is over
      if((millis() - lastFrameDisplayTime) > currentFrameDelay) {
        if(playNextGif)
        {
          bool fileOpened = false;

          if (gifIndex < num_files_Memory) {
            fileOpened = true;
          }

          if (((gifIndex >= num_files_Memory) && (gifIndex < (num_files_Memory + num_files_LittleFS))) &&
            openGifFilenameByIndex(LITTLEFS, GIF_DIRECTORY_LITTLEFS, (gifIndex - num_files_Memory), true, "LittleFS") >= 0) {
            fileOpened = true;
          }

          #ifdef AGIFS_USE_SD
            if (((gifIndex >= num_files_Memory + num_files_LittleFS) && (gifIndex < (num_files_Memory + num_files_LittleFS + num_files_SD))) &&
              openGifFilenameByIndex(SD, GIF_DIRECTORY_SD, (gifIndex - num_files_Memory - num_files_LittleFS), true, "SD") >= 0) {
              fileOpened = true;
            }
          #endif

          if(fileOpened) {
            int result;

            // try to start decoding GIF.  If there's any error, conditions are set up to increment gifIndex and try again on the next loop()
            if (gifIndex < num_files_Memory) {
              Serial.print("Pathname: Memory:");
              // If there's a file name to print, print it
              if(gifIndex < sizeof(gifsNameList) && gifsNameList[gifIndex] && strlen(gifsNameList[gifIndex])) {
                Serial.println(gifsNameList[gifIndex]);
              } else {
                Serial.print("gifsList[");
                Serial.print(gifIndex);
                Serial.println("]");
              }
              result = decoder.startDecoding((uint8_t *)gifsList[gifIndex], gifsSizeList[gifIndex]);
            } else {
              Serial.print("startDecoding:");
              Serial.println(gifIndex);
              result = decoder.startDecoding();
            }

            if (result >= 0) {
              // good result, clear playNextGif so we know to decode a frame, and don't load a new GIF on the next pass
              playNextGif = false;

              // Calculate time in the future to terminate animation
              displayStartTime_millis = now;          
            }
          }

          // get the gifIndex for the next pass through
          if (++gifIndex >= (num_files_SD + num_files_LittleFS + num_files_Memory)) {
            gifIndex = 0;
          }
        }

        if(!playNextGif) {
          // decode frame, false == don't delay after decode
          int result = decoder.decodeFrame(false);

          lastFrameDisplayTime = now;
          currentFrameDelay = decoder.getFrameDelay_ms();

          // it's time to start decoding a new GIF if there was an error, without any delay
          if(result < 0) {
            playNextGif = true;
            currentFrameDelay = 0;
          }
        }
      }
    }

    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject("agif");
      top["vsW"] = gifMatrixWidth;
      top["vsH"] = gifMatrixHeight;
      top["dtSec"] = displayTimeSeconds;
      top["numCyc"] = numFullCycles;
      top["playMode"] = playMode;
      top["sdClk"] = agifSdPinClk;
      top["sdMiso"] = agifSdPinMiso;
      top["sdMosi"] = agifSdPinMosi;
      top["sdCs"] = agifSdPinCs;
    }

    void readFromConfig(JsonObject& root)
    {
      JsonObject top = root["agif"];

      // If not all of the keys are present in the config, set a flag to write them later
      configExists = root.containsKey("agif") &&
                      top.containsKey("vsW") &&
                      top.containsKey("vsH") &&
                      top.containsKey("dtSec") &&
                      top.containsKey("numCyc") &&
                      top.containsKey("playMode") &&
                      top.containsKey("sdClk") &&
                      top.containsKey("sdMiso") &&
                      top.containsKey("sdMosi") &&
                      top.containsKey("sdCs");

      gifMatrixWidth = top["vsW"] | AGIF_MATRIX_WIDTH; //The value right of the pipe "|" is the default value in case your setting was not present in cfg.json (e.g. first boot)
      gifMatrixHeight = top["vsH"] | AGIF_MATRIX_HEIGHT;
      displayTimeSeconds = top["dtSec"] | DISPLAY_TIME_SECONDS;
      numFullCycles = top["numCyc"] | NUMBER_FULL_CYCLES;
      playMode = top["playMode"] | PLAYBACK_MODE;
      agifSdPinClk = top["sdClk"] | -1;
      agifSdPinMiso = top["sdMiso"] | -1;
      agifSdPinMosi = top["sdMosi"] | -1;
      agifSdPinCs = top["sdCs"] | -1;
    }

  private:
    //Private class members. You can declare variables and functions only accessible to your usermod here
    unsigned long lastTime = 0;
    bool configExists;
};
