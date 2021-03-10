#pragma once

typedef enum {
    playModeFirst = 0,
    playModeBoth = 1
} gifPlayModes;

void * agifsGetVirtualScreen(uint16_t width, uint16_t height);

void agifsSetNextGifIndex(int index, bool switchImmediately = false);

uint32_t agifsGetNextFrameDisplayTime();
uint16_t agifsGetMillisToNextFrameDisplay();
uint16_t agifsGetCurrentFrameDelay();
