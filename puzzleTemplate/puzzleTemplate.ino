#include <Arduino.h>
#include <Sleep_n0m1.h>
#include "lcd.h"
#include "config.h"
#include "messages.h"
#include "led.h"
#include "watchdog.h"
#include "puzzle_keypad.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <FastLED.h>
#include <Keypad.h>

Sleep sleep;
unsigned long sleepTime = 1000; //how long you want the arduino to sleep
#define RST_PIN         10

#define Sprintln(a) (Serial.println(F(a)))

#define SHOW_BOOT_PROMPT 0
#define IDLE_STATE 1
#define DISPLAY_IDLE 2
#define WAIT_FOR_GAME_DONE 3
#define DISPLAY_CHOICE_PROMPT 4
#define WAIT_MORAL_CHOICE 5
#define WAIT_FOR_GAME_DONE_TAG 6

int checkin_fsm_state = IDLE_STATE;

byte playerUid[4];
byte playerName[16];
byte playerData[16];
const int playerPacketSize = sizeof(playerUid) + sizeof(playerName) + sizeof(playerData);
byte playerPacket[playerPacketSize];

bool gameDone = false;
int choiceEntered = 0;
unsigned long idle_demo_timer = 0;


void setup() {
    Serial.begin(9600); // Initialize serial communications with the PC
    rfidSetup();
    lcdReset();
    ledSetup();
    setupButton();
    buttonOff();
    StartWatchdogTimer();
    checkin_fsm_state = IDLE_STATE;
    idle_demo_timer = millis();
}

void sleepNow() {
    delay(100);
    digitalWrite(RST_PIN, LOW);
    resetLed();
    lcdReset();
    sleep.pwrDownMode(); //set sleep mode
    sleep.sleepDelay(sleepTime); //sleep for: sleepTime
    setup();
}

void process_state_machine()
{
    //state machine
    switch (checkin_fsm_state) {
        case IDLE_STATE:  //Starting state to wait for RFID tag
        {
            if(PUZZLE_DEBUG_MODE == 0) {
                if (millis() - idle_demo_timer <= IDLE_DEMO_TIME) {
                    ledSetState(LED_IDLE);
                } else {
                    ledSetState(LED_OFF);
                    sleepNow();
                    if (random(15) == 0) {
                        idle_demo_timer = millis();
                    }
                }
            }
            else
            {
                ledSetState((LED_IDLE));
            }
            if (rfidDetected(playerUid, playerName, playerData, CHECKIN_TRIES_BYTE)) {
                ResetWatchdogTimer();
                setupPuzzle((char *)playerName);
                checkin_fsm_state = WAIT_FOR_GAME_DONE;
            }
            else
            {
                closeRfid();
            }
            break;
        }
        case WAIT_FOR_GAME_DONE: {
            updateGameState();
            if (gameDone) {
                lcdDisplayOn();
                checkin_fsm_state = DISPLAY_CHOICE_PROMPT;
                setLcdMessage(moralChoicePrompt, 10, 3000);
            }
            break;
        }

        case DISPLAY_CHOICE_PROMPT:
        {
            if(updateLcdMessage()) {
                checkin_fsm_state = WAIT_MORAL_CHOICE;
            }
            break;
        }

        case WAIT_MORAL_CHOICE:
        {
            char key = keyCheckForKeyPress();
            if(key)
            {
                ResetWatchdogTimer();
                if(key == '1' or key == '2')
                {
                    // save the choice... '1' will be 0
                    // '2' will be 1
                    choiceEntered = key - '1';
                    setLcdMessage(puzzleCompleteInstructions, 2, 0);
                    updateLcdMessage();
                    checkin_fsm_state = WAIT_FOR_GAME_DONE_TAG;
                }
                else
                {
                    // If they press a different key... lets just show them
                    // the choices again!
                    checkin_fsm_state = DISPLAY_CHOICE_PROMPT;
                    setLcdMessage(moralChoicePrompt, 10, 3000);
                }
            }
            break;
        }

        case WAIT_FOR_GAME_DONE_TAG: {
            // Display name and ask to retag
            ledSetState(LED_VICTORY);
            byte oldPlayerName[16];
            byte oldPlayerData[16];
            byte writeTagDone[1];
            if (rfidDetected(playerUid, oldPlayerName, oldPlayerData, CHECKIN_DONE_BYTE)) {
                // This assumes 1 is "paragon" path and 2 is "renegade"
                // this should be different for different puzzles so
                // people don't know exactly what they are picking
                if(choiceEntered = 0) {
                    if (!incrementBlockRfid(PARAGON_CHOICE_COUNT_BYTE)) {
                        closeRfid();
                        break;
                    }
                }
                else
                {
                    if (!incrementBlockRfid(RENEGADE_CHOICE_COUNT_BYTE))
                    {
                        closeRfid();
                        break;
                    }
                }
                ResetWatchdogTimer();
                lcdReset();
                checkin_fsm_state = IDLE_STATE;
                closeRfid();
            }
            else {
                closeRfid();
            }
            break;
        }
        default:
            break;
    }
}

void loop() {
    process_state_machine();
    Serial.println(checkin_fsm_state);
    ledUpdate();
    FastLED.show();
    if(WatchdogTimerExpired())
    {
        setup();
    }
    delay(GAME_TICK_RATE);
}
