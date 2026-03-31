#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "game_data.h"
#include "display_template.h"


// =======================
// 🖥️ DISPLAY / UI
// =======================

void updateDisplayQueue();
void forceDisplayTemplate(displayTemplate_t displayTemplate, uint32_t durationMillis);
void addDisplayTemplate(displayTemplate_t displayTemplate, uint32_t durationMillis);
bool isDisplayTemplateDonPlaying();


// =======================
// 🎮 GAME FLOW / LOGIC
// =======================

void resetGameLogic();
specialActies_t getSpecialActies();
void applyWrongAnswerPenalty();
void updateSpecialActies();

bool isWithinTimeLimit(void);
uint32_t getElapsedTime();
int32_t getTimeRemaining();

// =======================
// ⏱️ TIME SYSTEM
// =======================

void updateTimeGamePenaltyMillis();
uint32_t getWrongAnswerPenalty();

void updateVirtualTime();
uint32_t getVirtualElapsedTime();


void updateGameTimer();
void setGameTimer(int32_t sec);

// =======================
// 📍 GAME STATE / INPUT
// =======================

bool isInCorrectRoom(const char *beconIp);
bool isAnswerCorrect(char *userInput);
bool isInputMatching(const char *input, const char *correctInput);

uint8_t getNumRooms(void);



// =======================
// 🔐 HARDWARE / ACTIES
// =======================
void setRequiredSpecialActies(const specialActies_t required, const bool state);
compartment_t openCompartment(compartment_t compartment);
void setMapCoordinates(const uint8_t coordinates[2]);

// =======================
// 📡 COMMUNICATIE
// =======================

void send_run_data_to_esp(void);
void receive_room_settings_from_esp(void);


#endif