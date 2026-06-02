#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "game_data.h"
#include "display_template.h"
#include "board.h"

// =======================
// 🖥️ DISPLAY / UI
// =======================

void printInput(char *input, uint8_t len);
void updateDisplayQueue(void);
void forceDisplayTemplate(displayTemplate_t displayTemplate, uint32_t durationMillis);
void addDisplayTemplate(displayTemplate_t displayTemplate, uint32_t durationMillis);
bool isDisplayTemplateDonePlaying(void);

// =======================
// 🎮 GAME FLOW / LOGIC
// =======================

void resetGameLogic(void);
specialActies_t getSpecialActies(void);
void applyWrongAnswerPenalty(void);
void updateSpecialActies(void);

bool isWithinTimeLimit(void);
uint32_t getElapsedTime(void);
int32_t getTimeRemaining(void);
 bool isGameBizy(uint8_t *rIndex);
// =======================
// ⏱️ TIME SYSTEM
// =======================

void updateTimeGamePenaltyMillis(void);
uint32_t getWrongAnswerPenalty(void);

void updateVirtualTime(void);
uint32_t getVirtualElapsedTime(void);
void updateTimeDependingAudio(void);

void updateGameTimer(void);
void setGameTimer(int32_t sec);

// =======================
// 📍 GAME STATE / INPUT
// =======================

bool isInCorrectRoom(const char *beconIp);
bool isAnswerCorrect(char *userInput);
bool isInputMatching(const char *input, const char *correctInput);

uint8_t getNumRooms(void);


void send_run_data_to_esp_end(void);
void send_run_data_to_esp_start(void);
void send_run_data_to_esp_room(void);

// =======================
// 🔐 HARDWARE / ACTIES
// =======================
void setRequiredSpecialActies(const specialActies_t required, const bool state);
compartment_t openCompartment(compartment_t compartment);
void setMapCoordinates(const uint8_t coordinates[2]);

// =======================
// 📡 COMMUNICATIE
// =======================

void receive_room_settings_from_esp(void);

 void askRunData();
void printCustomDisplay(char *customDisplay);

static inline bool getPinState(const GPIO_Type *port, const uint8_t pin)
{
    return !((port->PDIR & (1<<pin)) == 0);
}

static inline void setPinState(GPIO_Type *port, const uint8_t pin, const bool value)
{
    if (value) 
    {
        port->PSOR = (1 << pin);
    } 
    else 
    {
        port->PCOR  = (1 << pin);
    }
}



#endif