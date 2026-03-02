#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H
#include "game_data.h"
#include <stdint.h>
#include <string.h>


void openCompartment(compartment_t compartment);

bool displayLoadTemplate(displayTemplate_t displayTemplate, uint32_t minDisplayTime, bool forceDisplay);

specialActies_t getSpecialActies();

static inline void applyWrongAnswerPenalty(void);

inline uint32_t getElapsedTime();

inline void updateTimeGamePanaltuMillis();

bool isInCorrectRoom(char *beconIp);

bool isAnswerCorrect(char *userInput);

bool isWithinTimeLimit(void);

uint32_t getWrongAnswerPenalty();

void setMapCoordinates(uint8_t coordinates[2]);

bool isInputMatching(char*, input,char* correctInput);

#endif