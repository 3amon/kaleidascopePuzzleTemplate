//
// Created by Lasciel on 8/13/2016.
//
#include <FastLED.h>

#ifndef PUZZLETEMPLATE_LED_H
#define PUZZLETEMPLATE_LED_H

#define LED_PIN     A3
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define NUM_LEDS    59 //11 + 24 + 24
#define LED_BASE   11

#define BRIGHTNESS  100

#define LED_OFF 0
#define LED_IDLE 1
#define LED_VICTORY 2
#define LED_FLASH_GREEN 3
#define LED_FLASH_RED 4
#define LED_SPATIAL_PUZZLE 5

void ledUpdate();

void ledSetup();

void resetLed();

void ledSetState(int state);

bool ledGetFlashDone();

void setFlashCount(int count);

void spatialPuzzleNewBox(bool match);

#endif //PUZZLETEMPLATE_LED_H
