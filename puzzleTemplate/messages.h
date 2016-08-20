//
// Created by Lasciel on 8/13/2016.
//

#ifndef PUZZLETEMPLATE_MESSAGES_H
#define PUZZLETEMPLATE_MESSAGES_H

#include <Arduino.h>
#include "config.h"

#define ARRAY_SIZE( array ) sizeof( array ) / sizeof( array[0] )

char puzzleCompleteInstructions[2][LCD_LINE_BUFFER_LENGTH] = {
        ////////////////
        "Please retag",
        "your card!"
};

#endif //PUZZLETEMPLATE_MESSAGES_H
