//
// Created by Lasciel on 8/13/2016.
//

#ifndef PUZZLETEMPLATE_MESSAGES_H
#define PUZZLETEMPLATE_MESSAGES_H

#include <Arduino.h>

String bootPromptLine1 = "Use: +/-/.";

String bootPromptLines[] = {
        ////////////////
        "Start Puzzle",
        "Test LEDs",
        "Configure" };

String puzzlePrompt[] = {
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

String puzzle_password_prompt[] = {
        ////////////////
        "Enter password:",
        "<password>"
};

String moralChoicePrompt[] = {
        ////////////////
        "Now use the",
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

String puzzleCompleteInstructions[] = {
        ////////////////
        "Please retag",
        "your card!"
};

#endif //PUZZLETEMPLATE_MESSAGES_H
