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

#if PUZZLE_NUM == 4
#include "messages_puzzle4.h"
#endif

Sleep sleep;
unsigned long sleepTime = 1000; //how long you want the arduino to sleep
#define RST_PIN         10

#define SLEEP_STATE 1
#define DISPLAY_IDLE 2
#define WAIT_FOR_GAME_DONE 3
#define DISPLAY_CHOICE_PROMPT 4
#define WAIT_MORAL_CHOICE 5
#define WAIT_FOR_GAME_DONE_TAG 6
#define WAIT_IDLE_LDE 7
#define WAIT_IDLE_OFF 8
#define WAIT_ADDITIONAL_INFO 9
#define DEBUG_STATE 99

int checkin_fsm_state = WAIT_IDLE_OFF;

byte playerUid[4];
byte playerName[16];
byte playerData[16];
const int playerPacketSize = sizeof(playerUid) + sizeof(playerName) + sizeof(playerData);
byte playerPacket[playerPacketSize];

int choiceEntered = 0;
unsigned long idle_demo_timer = 0;
int sleepCycles = 0;


void setup() {
    Serial.begin(9600); // Initialize serial communications with the PC
    rfidSetup();
    lcdReset();
    ledSetup();
    setupButton();
    buttonOff();

    if(PUZZLE_DEBUG_MODE == 0) {
        checkin_fsm_state = WAIT_IDLE_OFF;
        StartWatchdogTimer();
    }
    else
    {
        ledSetState(LED_CYCLE_GAME_SIDES);
        checkin_fsm_state = DEBUG_STATE;
    }
    idle_demo_timer = millis();
    randomSeed(analogRead(0));
}

void loop() {
    process_state_machine();
    ledUpdate();
    FastLED.show();
    if(WatchdogTimerExpired())
    {
        setup();
        Serial.println("Timer Expired!");
    }
    delay(GAME_TICK_RATE);
}

void process_state_machine()
{
    //state machine
    switch (checkin_fsm_state) {
        case SLEEP_STATE:  //Starting state to wait for RFID tag
        {

            ++sleepCycles;
            sleepNow();
            Serial.println("Sleeping!");
            if(sleepCycles >= 20)
            {
                ledSetState(LED_IDLE);
                checkin_fsm_state = WAIT_IDLE_LDE;
                idle_demo_timer = millis();
            }
            else
            {
                ledSetState(LED_OFF);
                checkin_fsm_state = WAIT_IDLE_OFF;
                idle_demo_timer = millis();
            }
            break;
        }
        case WAIT_IDLE_OFF:
        {
            if (millis() - idle_demo_timer > IDLE_OFF_TIME) {
                checkin_fsm_state = SLEEP_STATE;
            }
            process_rfid_checkIn();
            break;
        }
        case WAIT_IDLE_LDE: {
            if (millis() - idle_demo_timer > IDLE_DEMO_TIME) {
                sleepCycles = 0;
                checkin_fsm_state = SLEEP_STATE;
            }
            process_rfid_checkIn();
            break;
        }
        case WAIT_FOR_GAME_DONE: {
            if (updateGameState()) {
                ledSetState(LED_VICTORY);
                lcdDisplayOn();
                setLcdMessage(moralChoicePrompt, 10, DEFAULT_LCD_SCREEN_UPDATE);
                updateLcdMessage();
                checkin_fsm_state = DISPLAY_CHOICE_PROMPT;
                Serial.println("Game Done!");
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
                    setLcdMessage(moralChoicePrompt, 10, DEFAULT_LCD_SCREEN_UPDATE);
                }
            }
            break;
        }
        case WAIT_FOR_GAME_DONE_TAG: {
            // Display name and ask to retag
            process_rfid_checkOut();
            break;
        }
        case WAIT_ADDITIONAL_INFO:{
            if(updateLcdMessage()) {
                ledSetState(LED_OFF);
                lcdReset();
                checkin_fsm_state = SLEEP_STATE;
            }
            break;
        }
        case DEBUG_STATE: {

        }
        default:
            break;
    }
}

void process_rfid_checkIn()
{
    rfidSetup();
    if (rfidDetected(playerUid, playerName, playerData, PUZZLE_TRIES_BYTE)) {
        ResetWatchdogTimer();
        setupGame((char *)playerName);
        checkin_fsm_state = WAIT_FOR_GAME_DONE;
        Serial.println("Starting Game!");
    }
    closeRfid();
}

void process_rfid_checkOut()
{
    rfidSetup();
    byte oldPlayerName[16];
    byte oldPlayerData[16];
    byte writeTagDone[1];
    if (rfidDetected(playerUid, oldPlayerName, oldPlayerData, PUZZLE_COMPLETE_BYTE)) {
        // This assumes 1 is "paragon" path and 2 is "renegade"
        // this should be different for different puzzles so
        // people don't know exactly what they are picking
        if(choiceEntered = 0) {
            if (!incrementBlockRfid(PARAGON_CHOICE_COUNT_BYTE)) {
                return;
            }
        }
        else
        {
            if (!incrementBlockRfid(RENEGADE_CHOICE_COUNT_BYTE))
            {
                return;
            }
        }
        ResetWatchdogTimer();

        if(ADDITIONAL_INSTRUCTIONS) {
            setLcdMessage(additionalInfoPrompt, 10, DEFAULT_LCD_SCREEN_UPDATE);
            checkin_fsm_state = WAIT_ADDITIONAL_INFO;
        }
        else {
            ledSetState(LED_OFF);
            lcdReset();
            checkin_fsm_state = SLEEP_STATE;
        }
        Serial.println("Tag Updated!");
    }
    closeRfid();
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
