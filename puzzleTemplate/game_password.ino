#include "config.h"
#ifdef PUZZLE_GAME_PASSWORD
#include <Arduino.h>

#define WAIT_GAME_PROMPT 0
#define WAIT_KEY_INPUT 1
#define WAIT_PASSWORD_RESPONSE 2
#define GAME_DONE 2
#define NULL_STATE 100

static uint8_t puzzle_state;
static int puzzle_score;
static bool puzzle_match;
unsigned long puzzle_timer = millis();

#include "lcd.h"
#include "puzzle_keypad.h"
#include "watchdog.h"
#include "led.h"
#include "messages.h"

#define PASSWORD_LENGTH 5
//Note: This must be numbers!
#define PASSWORD "12345"

char puzzlePrompt[2][LCD_LINE_BUFFER_LENGTH] = {
        ////////////////
        "Greetings:",
        "<name>"
};

char passwordPrompt[2][LCD_LINE_BUFFER_LENGTH] = {
        ////////////////
        "Enter Password:",
        "<************>"
};

char passwordGood[2][LCD_LINE_BUFFER_LENGTH] = {
        ////////////////
        "Password entry",
        "correct!"
};

char passwordBad[2][LCD_LINE_BUFFER_LENGTH] = {
        ////////////////
        "Wrong password",
        "guess again..."
};

char (*currentPassword)[LCD_LINE_BUFFER_LENGTH];
static int passwordIndex;
static bool passwordStateCorrect;

void setupGame(char * playerName)
{
    currentPassword = &passwordPrompt[1];
    puzzle_state = WAIT_GAME_PROMPT;
    ledSetState(LED_OFF);
    lcdDisplayOn();
    strncpy(puzzlePrompt[1], playerName, LCD_LINE_BUFFER_LENGTH);
    setLcdMessage(puzzlePrompt, ARRAY_SIZE(puzzlePrompt), DEFAULT_LCD_SCREEN_UPDATE);
    updateLcdMessage();
}

bool updatePassword(char key)
{
    if(passwordIndex < PASSWORD_LENGTH) {
        (*currentPassword)[passwordIndex] = key;
        ++passwordIndex;
        return passwordIndex >= PASSWORD_LENGTH;
    }
    else
    {
        return true;
    }
}

void resetPassword()
{
    passwordIndex = 0;
    memset(currentPassword, 0, LCD_LINE_BUFFER_LENGTH);
}

bool updateGameState() {
    switch (puzzle_state) {
        case WAIT_GAME_PROMPT: {
            if (updateLcdMessage()) {
                resetPassword();
                setLcdMessage(passwordPrompt, ARRAY_SIZE(passwordPrompt), 0);
                puzzle_state = WAIT_KEY_INPUT;
                updateLcdMessage();
            }
            break;
        }
        case WAIT_KEY_INPUT: {
            char key = keyCheckForKeyPress();
            if(key) {
                ResetWatchdogTimer();
                if(updatePassword(key)) {
                    if(strncmp(*currentPassword, PASSWORD, PASSWORD_LENGTH) == 0)
                    {
                        setLcdMessage(passwordGood, ARRAY_SIZE(passwordGood), DEFAULT_LCD_SCREEN_UPDATE);
                        passwordStateCorrect = true;

                    }
                    else
                    {
                        setLcdMessage(passwordBad, ARRAY_SIZE(passwordBad), DEFAULT_LCD_SCREEN_UPDATE);
                        passwordStateCorrect = false;
                    }
                    puzzle_state = WAIT_PASSWORD_RESPONSE;
                }
                else {
                    setLcdMessage(passwordPrompt, ARRAY_SIZE(passwordPrompt), 0);
                    updateLcdMessage();
                }
            }
            break;
        }
        case WAIT_PASSWORD_RESPONSE:
        {
            if(updateLcdMessage())
            {
                if(passwordStateCorrect)
                {
                    return true;
                }
                else
                {
                    resetPassword();
                    setLcdMessage(passwordPrompt, ARRAY_SIZE(passwordPrompt), 0);
                    updateLcdMessage();
                    puzzle_state = WAIT_KEY_INPUT;
                }
            }
            break;
        }
        default:
            break;
    }
    return false;
}
#endif