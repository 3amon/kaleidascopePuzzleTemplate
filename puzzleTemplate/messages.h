//
// Created by Lasciel on 8/13/2016.
//

#ifndef PUZZLETEMPLATE_MESSAGES_H
#define PUZZLETEMPLATE_MESSAGES_H

#include <Arduino.h>

#define ARRAY_SIZE( array ) sizeof( array ) / sizeof( array[0] )

char puzzlePrompt[8][LCD_LINE_BUFFER_LENGTH] = {
        ////////////////
        "Greetings:",
        "<player_name>",
        "Press the green",
        "button when a",
        "cube is shown",
        "that could be",
        "made from the",
        "unfolded cross."
};

int puzzlePromptSize = ARRAY_SIZE(puzzlePrompt);

char puzzle_password_prompt[2][LCD_LINE_BUFFER_LENGTH] = {
        ////////////////
        "Enter password:",
        "<password>"
};

char moralChoicePrompt[10][LCD_LINE_BUFFER_LENGTH] = {
        ////////////////
        "Use the",
        "keypad to",
        "choose your",
        "path...",
        "Press 1:",
        "Save the puppy",
        "Press 2:",
        "Save the kitty",
        "Make your",
        "choice..."
};

char puzzleCompleteInstructions[2][LCD_LINE_BUFFER_LENGTH] = {
        ////////////////
        "Please retag",
        "your card!"
};

#endif //PUZZLETEMPLATE_MESSAGES_H
