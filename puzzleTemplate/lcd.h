//
// Created by Lasciel on 8/9/2016.
//

#ifndef PUZZLETEMPLATE_LCD_H
#define PUZZLETEMPLATE_LCD_H

#include <Arduino.h>

void lcdReset();

void lcdDisplayOn();

void setLcdMessage(String * lines, int numLines, unsigned long dly);

bool updateLcdMessage();

void lcdClear();

#endif //PUZZLETEMPLATE_LCD_H
