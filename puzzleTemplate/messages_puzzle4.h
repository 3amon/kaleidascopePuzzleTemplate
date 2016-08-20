//
// Created by Lasciel on 8/19/2016.
//

#ifndef PUZZLETEMPLATE_MESSAGES_PUZZLE4_H_H
#define PUZZLETEMPLATE_MESSAGES_PUZZLE4_H_H

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

#define ADDITIONAL_INSTRUCTIONS true

char additionalInfoPrompt[10][LCD_LINE_BUFFER_LENGTH] = {
        ////////////////
        "Psst...",
        "",
        "",
        "hey...",
        "Go to the ",
        "place",
        "With the guy",
        "",
        "Who has",
        "the thing"
};

#endif //PUZZLETEMPLATE_MESSAGES_PUZZLE4_H_H
