//
// Created by Lasciel on 8/11/2016.
//

#include "puzzle_keypad.h"
#include "config.h"
#include <Keypad.h>
#include "lcd.h"
#include <Arduino.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
        {'1','2','3','+'},
        {'4','5','6','-'},
        {'7','8','9','*'},
        {'.','0','=','/'}
};
byte rowPins[ROWS] = {A1, 0, 9, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

char keyCheckForKeyPress()
{
    return (char)customKeypad.getKey();
}