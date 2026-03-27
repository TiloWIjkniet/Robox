#ifndef DISPLAY_TEMPLATE_H
#define DISPLAY_TEMPLATE_H

#define MAX_TEMPLATES 20


#include <stdbool.h>
#include <stdint.h>

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
    D_FINAL_STEP,
    D_WRONG_ACTION,
    D_TIME_UP,
    D_WIN,

    // ERROR
    D_ERROR,

    D_MAX
}displayTemplate_t;

extern  const char *displayTemplates[MAX_TEMPLATES];
#endif