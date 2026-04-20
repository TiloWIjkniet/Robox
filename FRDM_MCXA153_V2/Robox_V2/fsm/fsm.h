#ifndef FSM_H
#define FSM_H

#include <stddef.h>
#include <stdint.h>

#include "states.h"
#include "events.h"

typedef struct
{
    void (*onEntry)(void);
    void (*onUpdate)(void);
    void (*onExit)(void);
}state_funcs_t;

typedef struct
{
    state_t from;
    event_t event;
    state_t to;

}transition_t;


/**
 * @brief Adds a state and its associated functions to the FSM.
 *
 * @param state The state identifier.
 * @param funcs Pointer to the state's function handlers.
 */
int FSM_addState(const state_t state, const state_funcs_t *funcs);

/**
 * @brief Adds a transition to the FSM.
 *
 * Stores the transition in the transition list.
 *
 * @param transition Pointer to the transition definition.
 */
int FSM_addTransition(const transition_t *transition);

/**
 * @brief Adds an event to the event buffer.
 *
 * Inserts the event into a circular buffer if space is available.
 *
 * @param event The event to add.
 */
int FSM_addEvent(const event_t event);


/**
 * @brief Updates the finite state machine (FSM).
 *
 * Processes one pending event (if available) and updates the current state.
 * Always calls the current state's onUpdate function if it exists.
 *
 * @note must by uset in main loop
 */
void FSM_update(void);

/**
 * @brief Initializes the FSM configuration.
 *
 * Registers all states with their handlers and defines all transitions.
 * Also queues the initial event to start the FSM.
 */
int FSM_init(void);

#define DEBUG_FSM 0
#if DEBUG_FSM
const char* STATE_NAMES[] = { "S_INIT", "S_IDLE", "S_DEV_PAGE", "S_FIRST_ROOM",
                             "S_ROOM_LOOP", "S_LAST_ROOM", "S_COMPLETED",
                             "S_TIMEOUT", "S_RESET" };

const char* EVENT_NAMES[] = { "E_INIT_COMPLETE", "E_START_DEV", "E_EXIT_DEV",
                             "E_START_GAME", "E_ROOM_COMPLETED", "E_ROOM_TIMEOUT",
                             "E_LAST_ROOM_COMPLETED", "E_ROOM_LOOP_TO_LAST",
                             "E_GAME_COMPLETED", "E_GAME_TIMEOUT", "E_RESET_COMPLETE" };
#endif

#endif