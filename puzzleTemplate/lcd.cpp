//
// Created by Lasciel on 8/9/2016.
//

#include "lcd.h"
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"
// Addr: 0x3F, 20 chars & 4 lines. Sometimes display boards use address 0x27
LiquidCrystal_I2C lcd(0x3F, 16, 2); //Frentally display, use 0x3F if not working try 0x27

int lcd_lineIndex = 0;
int lcd_numLines = 0;
unsigned long lcd_timer = 0UL;
unsigned long lcd_dly = 0UL;
String * lcd_lines = 0;

void lcdReset()
{
    lcd.clear();
    lcd.init();
    delay(100);
    lcd.noBacklight();
    lcd.setCursor(0, 0);
}

void lcdDisplayOn() {
    lcd.backlight();
}

void lcdClear()
{
    lcd.clear();
}

void setLcdMessage(String * lines, int numLines, unsigned long dly)
{
    lcd_timer = millis();
    lcd_lines = lines;
    lcd_numLines = numLines;
    lcd_lineIndex = 0;
    lcd_dly = dly;
}

bool updateLcdMessage()
{
    // Timer has expired for this line!
    if(millis() - lcd_timer > lcd_dly or lcd_lineIndex == 0)
    {
        lcd_timer = millis();

        if(lcd_lineIndex >= lcd_numLines)
        {
            return true;
        }
        lcdClear();

        for(int lineOffset = 0; lineOffset < LCD_LINES; ++ lineOffset)
        {
            if(lcd_lineIndex < lcd_numLines) {
                lcd.setCursor(0, lineOffset);
                lcd.print(lcd_lines[lcd_lineIndex]);
                lcd_lineIndex++;
            }
        }

    }
    return false;
}
