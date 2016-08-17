//
// Created by Lasciel on 8/9/2016.
//

#ifndef PUZZLETEMPLATE_LCD_H
#define PUZZLETEMPLATE_LCD_H

#include <Arduino.h>
#include "config.h"

void lcdReset();

void lcdDisplayOn();

void setLcdMessage(char lines[][LCD_LINE_BUFFER_LENGTH], int numLines, unsigned long dly);

bool updateLcdMessage();

void lcdClear();

#endif //PUZZLETEMPLATE_LCD_H
