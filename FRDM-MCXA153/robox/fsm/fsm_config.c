#include "fsm.h"
#include "state_logic.h"
#include <stddef.h>

/**
* @brief  Initialiseert de FSM door alle states en transitions toe te voegen.
*/
void FSM_config(void)
{
    //States
    FSM_addState(S_INIT,            &(state_funcs_t){NULL,              NULL,               NULL});
    FSM_addState(S_IDLE,            &(state_funcs_t){idle_onEntry,      idle_onUpdate,      idle_onExit});
    FSM_addState(S_DEV_PAGE,        &(state_funcs_t){dev_page_onEntry,  dev_page_onUpdate,  dev_page_onExit});
    FSM_addState(S_FIRST_ROOM,      &(state_funcs_t){first_room_onEntry,first_room_onUpdate,first_room_onExit});
    FSM_addState(S_ROOM_LOOP,       &(state_funcs_t){room_loop_onEntry, room_loop_onUpdate, room_loop_onExit});
    FSM_addState(S_LAST_ROOM,       &(state_funcs_t){last_room_onEntry, last_room_onUpdate, last_room_onExit});
    FSM_addState(S_COMPLETED,       &(state_funcs_t){completed_onEntry, completed_onUpdate, completed_onExit});
    FSM_addState(S_TIMEOUT,         &(state_funcs_t){timeout_onEntry,   timeout_onUpdate,   timeout_onExit});
    FSM_addState(S_RESET,           &(state_funcs_t){reset_onEntry,     reset_onUpdate,     ireset_onExit});

    //Transitions
    FSM_addTransition(&(transition_t){S_INIT,       E_INIT_COMPLETE,         S_IDLE});

    FSM_addTransition(&(transition_t){S_IDLE,       E_START_DEV,             S_DEV_PAGE});
    FSM_addTransition(&(transition_t){S_DEV_PAGE,   E_EXIT_DEV,              S_IDLE});

    //Kamer Tranisions
    FSM_addTransition(&(transition_t){S_IDLE,       E_START_GAME,            S_FIRST_ROOM}); //Start
    FSM_addTransition(&(transition_t){S_FIRST_ROOM, E_ROOM_COMPLETED,        S_ROOM_LOOP});  // Eerste kamer voltooid
    FSM_addTransition(&(transition_t){S_ROOM_LOOP,  E_ROOM_COMPLETED,        S_ROOM_LOOP});  // Tussenkamer voltooid
    FSM_addTransition(&(transition_t){S_ROOM_LOOP,  E_ROOM_LOOP_TO_LAST,     S_LAST_ROOM});  // Laatste kamer bereikt
    FSM_addTransition(&(transition_t){S_LAST_ROOM,  E_LAST_ROOM_COMPLETED,   S_COMPLETED});  // Laatste kamer voltooid
    FSM_addTransition(&(transition_t){S_COMPLETED,  E_GAME_COMPLETED,        S_RESET});
    FSM_addTransition(&(transition_t){S_RESET,      E_RESET_COMPLETE,        S_IDLE});

    FSM_addTransition(&(transition_t){S_FIRST_ROOM, E_ROOM_TIMEOUT,          S_TIMEOUT});    // Timeout eerste kamer
    FSM_addTransition(&(transition_t){S_ROOM_LOOP,  E_ROOM_TIMEOUT,          S_TIMEOUT});    // Timeout tussenkamer
    FSM_addTransition(&(transition_t){S_LAST_ROOM,  E_ROOM_TIMEOUT,          S_TIMEOUT});    // Timeout laatste kamer
    FSM_addTransition(&(transition_t){S_TIMEOUT,    E_GAME_TIMEOUT,          S_RESET});      

    FSM_addEvent(E_ROOM_TIMEOUT);
}

