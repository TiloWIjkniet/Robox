#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H
#include "game_data.h"
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "display_template.h"
compartment_t openCompartment(compartment_t compartment);

bool displayLoadTemplate(displayTemplate_t displayTemplate, uint32_t minDisplayTime, bool forceDisplay);

specialActies_t getSpecialActies();

void applyWrongAnswerPenalty();

uint32_t getElapsedTime();

void updateTimeGamePanaltuMillis();

bool isInCorrectRoom(char *beconIp);

bool isAnswerCorrect(char *userInput);

bool isWithinTimeLimit(void);

uint32_t getWrongAnswerPenalty();

void setMapCoordinates(uint8_t coordinates[2]);

bool isInputMatching(const  char *input, const char *correctInput);

uint8_t getNumRooms(void) ;
#endif