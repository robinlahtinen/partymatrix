//
// Created by Robin on 28.11.2022.
//

#ifndef PARTYMATRIX_MAIN_H
#define PARTYMATRIX_MAIN_H

extern const int NUM_LEDS;
extern CRGB leds[];

void initFastLED();

void initPins();

void initButtons();

bool isDebug();

int getMicReading();

int getSimulatedMicReading();

void doResetLeds();

void doButtonUpdate();

void doDebugPrint();

#endif //PARTYMATRIX_MAIN_H
