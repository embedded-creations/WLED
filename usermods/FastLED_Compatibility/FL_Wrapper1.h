#pragma once

typedef struct xyPair {
  uint16_t x;
  uint16_t y;
} xyPair;

typedef struct fXyPair {
  float x;
  float y;
} fXyPair;

typedef struct Segmentmap {
  uint16_t minVirtualScreenWidth;
  uint16_t minVirtualScreenHeight;
  uint16_t numElements;
  char * mapname;
  xyPair * xyTablePointer;
} segmentmap;

extern segmentmap _segmentmaps[MAX_NUM_SEGMENTS];
extern uint16_t kMatrixWidth;
extern uint16_t kMatrixHeight;

uint16_t XY( uint8_t x, uint8_t y);

// allocate space for lookup table and populate at the beginning of each effect?
  // how slow is this?
  // what's the alternative?  Need to check for valid data at startup anyway

// what if virtual screen is already allocated?

// END_FASTLED() check for valid lookuptable pointer and call appropriate writeLeds() function?
  // use kMatrixWidth for this?

// AGifs.getVirtualScreen(**CRGB pointer, &width, &height)

// ALLOC_VIRTUAL_SCREEN() before ALLOC()?



extern int NUM_LEDS;
extern CRGB * leds;
extern void * flStructPtr;

#define FL_ALLOC_WITH_1_ARRAY_XY(FL_STRUCT_CONTENTS, TYPE1, NAME1, NUM_ELEMENTS1)                       \
  kMatrixWidth = _segmentmaps[_segment_index].minVirtualScreenWidth;                                    \
  kMatrixHeight = _segmentmaps[_segment_index].minVirtualScreenHeight;                                  \
  NUM_LEDS = (kMatrixWidth*kMatrixHeight);                                                              \
  if(!NUM_LEDS) return mode_static();                                                                   \
  struct fl_Struct{ FL_STRUCT_CONTENTS TYPE1 * NAME1; };                                                \
  int flAllocSize = (sizeof(CRGB) * NUM_LEDS) + sizeof(fl_Struct) + (sizeof(TYPE1) * NUM_ELEMENTS1);    \
  if(!SEGENV.allocateData(flAllocSize)) return mode_static();                                           \
  uint8_t * segmentDataPtr = SEGENV.data;                                                               \
  leds = reinterpret_cast<CRGB*>(segmentDataPtr);                                                       \
  segmentDataPtr += (sizeof(CRGB) * NUM_LEDS);                                                          \
  flStructPtr = segmentDataPtr;                                                                         \
  segmentDataPtr += sizeof(fl_Struct);                                                                  \
  TYPE1 * NAME1 = (TYPE1 *)(segmentDataPtr);                                                            \
  ((fl_Struct*)flStructPtr)->NAME1 = NAME1;                                                             \

#define FL_ALLOC(FL_STRUCT_CONTENTS)                                  \
  NUM_LEDS = SEGLEN;                                                  \
  struct fl_Struct{ FL_STRUCT_CONTENTS };                             \
  int flAllocSize = (sizeof(CRGB) * NUM_LEDS) + sizeof(fl_Struct);    \
  if(!SEGENV.allocateData(flAllocSize)) return mode_static();         \
  uint8_t * segmentDataPtr = SEGENV.data;                             \
  leds = reinterpret_cast<CRGB*>(segmentDataPtr);                     \
  segmentDataPtr += (sizeof(CRGB) * NUM_LEDS);                        \
  flStructPtr = segmentDataPtr;                                       \

#define FL_STATICVAR(TESTVAR) (((fl_Struct*)flStructPtr)->TESTVAR)

#define BEGIN_FASTLED_XY_COMPATIBILITY()                                          \
          kMatrixWidth = _segmentmaps[_segment_index].minVirtualScreenWidth;      \
          kMatrixHeight = _segmentmaps[_segment_index].minVirtualScreenHeight;    \
          NUM_LEDS = (kMatrixWidth*kMatrixHeight);                                \
          if(!NUM_LEDS) return mode_static();                                     \
          if(!SEGENV.allocateData(sizeof(CRGB) * NUM_LEDS)) return mode_static(); \
          leds = reinterpret_cast<CRGB*>(SEGENV.data);                            \

#define END_FASTLED_XY_COMPATIBILITY()    \
          writeLedsArrayToWled_XY(leds);  \

#define BEGIN_FASTLED_COMPATIBILITY()                                             \
          NUM_LEDS = SEGLEN;                                                      \
          if(!SEGENV.allocateData(sizeof(CRGB) * NUM_LEDS)) return mode_static(); \
          leds = reinterpret_cast<CRGB*>(SEGENV.data);                            \

#define END_FASTLED_COMPATIBILITY()       \
          writeLedsArrayToWled(leds);     \

#define FL_FUNCTIONS_BEGIN struct fl_Functions { 
#define FL_FUNCTIONS_END  }; fl_Functions flFunctions;
#define FL_FUNCTION(FUNCTION) flFunctions.FUNCTION 

#define FL_EVERY_N(NAME,N) \
  if(!SEGENV.call) {((fl_Struct*)flStructPtr)->NAME.reset(); ((fl_Struct*)flStructPtr)->NAME.setPeriod(N); } \
  if(((fl_Struct*)flStructPtr)->NAME)

#define FL_EVERY_N_MILLIS       FL_EVERY_N
#define FL_EVERY_N_MILLISECONDS FL_EVERY_N
#define FL_EVERY_N_SECONDS      FL_EVERY_N
#define FL_EVERY_N_BSECONDS     FL_EVERY_N
#define FL_EVERY_N_MINUTES      FL_EVERY_N
#define FL_EVERY_N_HOURS        FL_EVERY_N

#define FL_SETUP() if(!SEGENV.call)
