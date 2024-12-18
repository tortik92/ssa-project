#pragma once

#ifndef PIO_UNIT_TESTING
#include <Arduino.h>
#endif

// BLEComm inputs
const uint8_t phoneInput_makeSound_pad1 = 0x01;
const uint8_t phoneInput_makeSound_pad2 = 0x02;
const uint8_t phoneInput_makeSound_pad3 = 0x03;
const uint8_t phoneInput_makeSound_pad4 = 0x04;
const uint8_t phoneInput_gameSelection_Memory = 0x10;
const uint8_t phoneInput_gameSelection_Reaktion = 0x11;
const uint8_t phoneInput_cancelGame = 0xFF;

// PadsComm I/0
const uint8_t padOutput_playSound8Val = 0x20;
const uint8_t padOutput_waitForPlayerOnPad = 0x30;
const uint8_t padOutput_cancelOperation = 0xFF;

const uint8_t padInput_padOccupied = 0x01;

// game constants
const uint8_t maxAllowedPads = 3;
const uint8_t reaktionPlayerCount = 2;
const uint8_t paramLen = 8;
const uint8_t chordLen = 4;
const unsigned long defaultDelay = 7000; // 7 seconds

// music
const int gameActionTones[paramLen] = {440, 550, 660, 880, 0, 0, 0, 0};
const int gameActionDurations[paramLen] = {100, 100, 100, 200, 0, 0, 0, 0};

const int gameConclusionTones[paramLen] = {440, 550, 660, 550, 660, 880, 0, 0};
const int gameConclusionDurations[paramLen] = {125, 125, 125, 125, 125, 125, 0, 0};

const int soundsArray[paramLen] = {880, 1760, 2640, 3520, 0, 0, 0, 0}; // tone values in Hz, 4 times zero to make array length of 8
const int defaultBeat[paramLen] = {200, 200, 400, 400, 200, 200, 0, 0};

// lexer
const size_t maxTokens = 256;
const uint8_t intAsStrLen = 10;