#ifndef DISPLAY_TEMPLATE_H
#define DISPLAY_TEMPLATE_H

#define MAX_TEMPLATES 40

#define DISPLAY_5S 5000
#define DISPLAY_3S 3000
#define DISPLAY_LEN 150
#include <stdbool.h>
#include <stdint.h>
    // TODO: Maak de startfunctie af
    // NOTE: Dit is alleen een test-commentaar
    // WARN: Let op, deze functie kan crashen
    // IDEA: Misschien kunnen we dit optimaliseren
    // BUG: Fout bij indexberekening
    // DONE: Deze functie is afgerond
    // DOING: Hier ben ik nu mee bezig
    // TEMP: Tijdelijke code, verwijderen later
    // TEST: Test deze functie grondig
    // FIXME: Hier zit een bug die gefixt moet worden
    // OPTIMIZE: Code kan sneller worden gemaakt
    // REVIEW: Laat iemand anders dit controleren
    // HACK: Snel opgelost, maar niet ideaal
    // DEBUG: Print debug informatie
    // QUESTION: Waarom doet dit niet wat verwacht?
typedef enum
{
    D_NONE = 0,

    // MAIN STATES
    D_IDLE,
    D_DEV_PAGE,
    D_START_GAME,

    // NAVIGATION
    D_GO_TO_ROOM_1,
    D_GO_TO_ROOM_2,
    D_GO_TO_ROOM_3,
    D_GO_TO_FINAL_ROOM,

    // ROOM STATES
    D_IN_ROOM,
    D_FINAL_ROOM_ENTER,

    // INPUT
    D_ENTER_ANSWER,
    D_ENTER_ANSWER_STRESS,

    // CORRECT
    D_CORRECT_1,
    D_CORRECT_2,
    D_CORRECT_3,
    D_FIRST_SUCCESS,

    // WRONG
    D_WRONG_1,
    D_WRONG_2,
    D_WRONG_3,

    // ACTIONS
    D_SCAN,
    D_KEY,
    D_SWITCH,
    D_SPECIAL_WRONG,
    D_SPECIAL_CORRECT,
    
    D_OPEN_COMPARTMENT,

    // END GAME FLOW
    D_TIME_UP,
    D_WIN,

    // ERROR
    D_ERROR,
    D_ERROR_SWITCH_OR_KEY_NEETST_TO_BE_RESET,
    D_MAX
}displayTemplate_t;

extern  const char displayTemplatesNL[MAX_TEMPLATES][DISPLAY_LEN];
extern  const char displayTemplatesSafeNL[MAX_TEMPLATES][DISPLAY_LEN];
extern  const char displayTemplatesEn[MAX_TEMPLATES][DISPLAY_LEN];
extern  const char displayTemplatesSafeEn[MAX_TEMPLATES][DISPLAY_LEN];
#endif