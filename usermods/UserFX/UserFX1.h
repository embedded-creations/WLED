#pragma once

// This file can be included into FX.h by defining USERFX1_H with the path+name to this file in quotes

// This is the count of how many modes are in this UsermodFX set
#define USERFX1_MODE_COUNT 1

// give each mode a unique definition and number ascending from 0
#define USERFX1_MODE_BLINK2                    0

// map one mode per line, with a slash at the end of each line
#define USERFX1_ADD_MODES_TO_MAP()       \
    _mode[BUILTIN_MODE_COUNT + USERFX1_MODE_BLINK2]                   = &WS2812FX::mode_blink2; \

// on each line: prepend comma before mode, add mode function declaration, followed by a slash
#define USERFX1_MODES_LIST()    \
    ,mode_blink2(void)          \

// prepend comma before each mode name, no comma after last name
#define USERFX1_JSON_MODE_NAMES R"=====(,"blink 2")====="
