#pragma once

// This file can be included into FX.h by defining USERFX2_H with the pathname to this file in quotes

// This is the count of how many modes are in the FX_user 
#define USERFX2_MODE_COUNT 35

// give each mode a unique number ascending from 0
#define USERFX2_MODE_BLINK3                    0
#define FX_MODE_PIXELS                         1
#define FX_MODE_PIXELWAVE                      2
#define FX_MODE_JUGGLES                        3
#define FX_MODE_MATRIPIX                       4
#define FX_MODE_GRAVIMETER                     5
#define FX_MODE_PLASMOID                       6
#define FX_MODE_PUDDLES                        7
#define FX_MODE_MIDNOISE                       8
#define FX_MODE_NOISEMETER                     9
#define FX_MODE_FREQWAVE                      10
#define FX_MODE_FREQMATRIX                    11
#define FX_MODE_2DGEQ                         12
#define FX_MODE_WATERFALL                     13
#define FX_MODE_FREQPIXELS                    14
#define FX_MODE_BINMAP                        15
#define FX_MODE_NOISEFIRE                     16
#define FX_MODE_PUDDLEPEAK                    17
#define FX_MODE_NOISEMOVE                     18
#define FX_MODE_2DPLASMA                      19
#define FX_MODE_PERLINMOVE                    20
#define FX_MODE_RIPPLEPEAK                    21
#define FX_MODE_2DFIRENOISE                   22
#define FX_MODE_2DSQUAREDSWIRL                23
#define FX_MODE_2DFIRE2012                    24
#define FX_MODE_2DDNA                         25
#define FX_MODE_2DMATRIX                      26
#define FX_MODE_2DMEATBALLS                   27
#define FX_MODE_FREQMAP                       28
#define FX_MODE_GRAVCENTER                    29
#define FX_MODE_GRAVCENTRIC                   30
#define FX_MODE_GRAVFREQ                      31
#define FX_MODE_DJLIGHT                       32
#define FX_MODE_2DFUNKYPLANK                  33
#define FX_MODE_2DCENTERBARS                  34

// map one mode per line, with a slash at the end of each line
#define USERFX2_ADD_MODES_TO_MAP()       \
    _mode[BUILTIN_MODE_COUNT + USERFX2_MODE_BLINK3]                 = &WS2812FX::mode_blink3;           \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_PIXELS]                      = &WS2812FX::mode_pixels;           \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_PIXELWAVE]                   = &WS2812FX::mode_pixelwave;        \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_JUGGLES]                     = &WS2812FX::mode_juggles;          \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_MATRIPIX]                    = &WS2812FX::mode_matripix;         \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_GRAVIMETER]                  = &WS2812FX::mode_gravimeter;       \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_PLASMOID]                    = &WS2812FX::mode_plasmoid;         \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_PUDDLES]                     = &WS2812FX::mode_puddles;          \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_MIDNOISE]                    = &WS2812FX::mode_midnoise;         \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_NOISEMETER]                  = &WS2812FX::mode_noisemeter;       \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_FREQWAVE]                    = &WS2812FX::mode_freqwave;         \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_FREQMATRIX]                  = &WS2812FX::mode_freqmatrix;       \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_2DGEQ]                       = &WS2812FX::mode_2DGEQ;            \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_WATERFALL]                   = &WS2812FX::mode_waterfall;        \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_FREQPIXELS]                  = &WS2812FX::mode_freqpixels;       \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_BINMAP]                      = &WS2812FX::mode_binmap;           \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_NOISEFIRE]                   = &WS2812FX::mode_noisefire;        \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_PUDDLEPEAK]                  = &WS2812FX::mode_puddlepeak;       \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_NOISEMOVE]                   = &WS2812FX::mode_noisemove;        \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_2DPLASMA]                    = &WS2812FX::mode_2Dplasma;         \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_PERLINMOVE]                  = &WS2812FX::mode_perlinmove;       \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_RIPPLEPEAK]                  = &WS2812FX::mode_ripplepeak;       \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_2DFIRENOISE]                 = &WS2812FX::mode_2Dfirenoise;      \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_2DSQUAREDSWIRL]              = &WS2812FX::mode_2Dsquaredswirl;   \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_2DFIRE2012]                  = &WS2812FX::mode_2Dfire2012;       \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_2DDNA]                       = &WS2812FX::mode_2Ddna;            \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_2DMATRIX]                    = &WS2812FX::mode_2Dmatrix;         \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_2DMEATBALLS]                 = &WS2812FX::mode_2Dmeatballs;      \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_FREQMAP]                     = &WS2812FX::mode_freqmap;          \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_GRAVCENTER]                  = &WS2812FX::mode_gravcenter;       \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_GRAVCENTRIC]                 = &WS2812FX::mode_gravcentric;      \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_GRAVFREQ]                    = &WS2812FX::mode_gravfreq;         \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_DJLIGHT]                     = &WS2812FX::mode_DJLight;          \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_2DFUNKYPLANK]                = &WS2812FX::mode_2DFunkyPlank;     \
    _mode[BUILTIN_MODE_COUNT + FX_MODE_2DCENTERBARS]                = &WS2812FX::mode_2DCenterBars;     \

// on each line: prepend comma before mode, add mode function declaration, followed by a slash
#define USERFX2_MODES_LIST()    \
    ,mode_blink3(void)          \
    ,mode_pixels(void)          \
    ,mode_pixelwave(void)       \
    ,mode_juggles(void)         \
    ,mode_matripix(void)        \
    ,mode_gravimeter(void)      \
    ,mode_plasmoid(void)        \
    ,mode_puddles(void)         \
    ,mode_midnoise(void)        \
    ,mode_noisemeter(void)      \
    ,mode_freqwave(void)        \
    ,mode_freqmatrix(void)      \
    ,mode_2DGEQ(void)           \
    ,mode_waterfall(void)       \
    ,mode_freqpixels(void)      \
    ,mode_binmap(void)          \
    ,mode_noisefire(void)       \
    ,mode_puddlepeak(void)      \
    ,mode_noisemove(void)       \
    ,mode_2Dplasma(void)        \
    ,mode_perlinmove(void)      \
    ,mode_ripplepeak(void)      \
    ,mode_2Dfirenoise(void)     \
    ,mode_2Dsquaredswirl(void)  \
    ,mode_2Dfire2012(void)      \
    ,mode_2Ddna(void)           \
    ,mode_2Dmatrix(void)        \
    ,mode_2Dmeatballs(void)     \
    ,mode_freqmap(void)         \
    ,mode_gravcenter(void)      \
    ,mode_gravcentric(void)     \
    ,mode_gravfreq(void)        \
    ,mode_DJLight(void)         \
    ,mode_2DFunkyPlank(void)    \
    ,mode_2DCenterBars(void)    \

// prepend comma before each mode name, no comma after last name
#define USERFX2_JSON_MODE_NAMES R"=====(,"blink 3")=====" \
    R"=====(,"* Pixels","* Pixelwave","* Juggles","* Matripix","* Gravimeter","* Plasmoid","* Puddles","* Midnoise","* Noisemeter","** Freqwave","** Freqmatrix","** 2D GEQ","** Waterfall","** Freqpixels","** Binmap","* Noisefire","* Puddlepeak","** Noisemove","2D Plasma","Perlin Move","* Ripple Peak","2D FireNoise","2D Squared Swirl","2D Fire2012","2D DNA","2D Matrix","2D Meatballs","** Freqmap","* Gravcenter","* Gravcentric","** Gravfreq","** DJ Light","** 2D Funky Plank","** 2D CenterBars")====="