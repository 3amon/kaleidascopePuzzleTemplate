//
// Created by Lasciel on 8/10/2016.
//

#ifndef PUZZLETEMPLATE_CONFIG_H
#define PUZZLETEMPLATE_CONFIG_H

#define KEYPAD_MAX_INPUT_LENGTH 7
#define LCD_LINES 2

#define PUZZLE_DEBUG_MODE 0

#define CHECKIN_TRIES_BYTE 0
#define SPEECH_TRIES_BYTE 1
#define LIGHT_TRIES_BYTE 2
#define CHECKIN_DONE_BYTE 4
#define SPEECH_DONE_BYTE 5
#define NAME_RFID_BLOCK 4
#define DATA_RFID_BLOCK 5

// putting these at the end!
#define RENEGADE_CHOICE_COUNT_BYTE 14
#define PARAGON_CHOICE_COUNT_BYTE 15

#define PUZZLE_PASSWORD "12345"

#define IDLE_DEMO_TIME 5000UL

#define WATCHDOG_TIMEOUT 50000UL

#define GAME_TICK_RATE 100

#define LED_FLASH_RATE 500UL

#define GAME_MATCH_REQ 5

#endif //PUZZLETEMPLATE_CONFIG_H
