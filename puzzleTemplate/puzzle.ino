#include <Arduino.h>

#define WAIT_PUZZLE_PROMPT 0
#define PUZZLE_RUNNING 1
#define PUZZLE_DONE 2
#define SHOW_RESULT 3
#define NULL_STATE 100

static uint8_t puzzle_state;
static int score;

#include "lcd.h"
#include "config.h"
#include "puzzle_keypad.h"
#include "watchdog.h"
#include "led.h"

void setupPuzzle(char * playerName)
{
    score = 0;
    puzzle_state = WAIT_PUZZLE_PROMPT;
    gameDone = false;
    keyClearData();
    puzzlePrompt[1] = String(playerName);
    lcdDisplayOn();
    setLcdMessage(puzzlePrompt, 8, 3000);
    updateLcdMessage();
}

void updateGameState() {
    switch (puzzle_state) {
        case WAIT_PUZZLE_PROMPT: {
            if (updateLcdMessage()) {
                buttonOn();
                puzzle_state = PUZZLE_RUNNING;
                lcdReset();
            }
            break;
        }
        case PUZZLE_RUNNING:
            if (buttonPressed()) {
                ResetWatchdogTimer();
                ++score;
                if (score >= GAME_MATCH_REQ) {
                    puzzle_state = PUZZLE_DONE;
                } else {
                    ledSetState(LED_FLASH_GREEN);
                    puzzle_state = SHOW_RESULT;
                    setFlashCount(score);
                }
            }
            break;
        case SHOW_RESULT:{
            if(ledGetFlashDone())
            {
                ledSetState(LED_OFF);
                puzzle_state = PUZZLE_RUNNING;
            }
            break;
        }
        case PUZZLE_DONE:
            gameDone = true;
            buttonOff();
            keyClearData();
            lcdReset();
            puzzle_state = NULL_STATE;
            break;
        default:
            break;
    }
}

