#include "config.h"
#ifdef PUZZLE_GAME_SPATIAL
#include <Arduino.h>

#define WAIT_PUZZLE_PROMPT 0
#define PUZZLE_RUNNING 1
#define PUZZLE_DONE 2
#define SHOW_RESULT 3
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

#define GAME_MATCH_REQ 5
#define GAME_BASE_UPDATE_RATE 3000UL

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

void setupGame(char * playerName)
{
    puzzle_score = 0;
    puzzle_state = WAIT_PUZZLE_PROMPT;
    ledSetState(LED_OFF);
    strncpy(puzzlePrompt[1], playerName, LCD_LINE_BUFFER_LENGTH);
    lcdDisplayOn();
    setLcdMessage(puzzlePrompt, puzzlePromptSize, 3000);
    updateLcdMessage();
}

void makeNewPuzzle()
{
    puzzle_match = random(100) >= 75;
    puzzle_timer = millis();
    spatialPuzzleNewBox(puzzle_match);
    ledSetState(LED_SPATIAL_PUZZLE);
}

bool updateGameState() {
    switch (puzzle_state) {
        case WAIT_PUZZLE_PROMPT: {
            if (updateLcdMessage()) {
                buttonOn();
                puzzle_state = PUZZLE_RUNNING;
                lcdReset();
                makeNewPuzzle();
            }
            break;
        }
        case PUZZLE_RUNNING:
            if (buttonPressed()) {
                ResetWatchdogTimer();
                if(puzzle_match) {
                    ++puzzle_score;
                    if(puzzle_score >= GAME_MATCH_REQ)
                    {
                        puzzle_state = PUZZLE_DONE;
                    }
                    else
                    {
                        setFlashCount(puzzle_score);
                        ledSetState(LED_FLASH_GREEN);
                        puzzle_state = SHOW_RESULT;
                    }
                }
                else
                {
                    setFlashCount(1);
                    puzzle_score = max(puzzle_score - 1, 0);
                    ledSetState(LED_FLASH_RED);
                    puzzle_state = SHOW_RESULT;
                }
            }
            else if (millis() - puzzle_timer > GAME_BASE_UPDATE_RATE)
            {
                makeNewPuzzle();
            }
            break;
        case SHOW_RESULT:{
            if(ledGetFlashDone())
            {
                makeNewPuzzle();
                puzzle_state = PUZZLE_RUNNING;
            }
            break;
        }
        case PUZZLE_DONE:
            buttonOff();
            puzzle_state = NULL_STATE;
            break;
        default:
            break;
    }

    return puzzle_state == PUZZLE_DONE;
}
#endif