#include <string.h>
#include <stdint.h>

#include "fsm.h"

#define DEBUG_FMS 1
#if DEBUG_FMS
#include <stdio.h>
const char* stateNames[] = { "S_INIT", "S_IDLE", "S_DEV_PAGE", "S_FIRST_ROOM",
                             "S_ROOM_LOOP", "S_LAST_ROOM", "S_COMPLETED",
                             "S_TIMEOUT", "S_RESET" };

const char* eventNames[] = { "E_INIT_COMPLETE", "E_START_DEV", "E_EXIT_DEV",
                             "E_START_GAME", "E_ROOM_COMPLETED", "E_ROOM_TIMEOUT",
                             "E_LAST_ROOM_COMPLETED", "E_ROOM_LOOP_TO_LAST",
                             "E_GAME_COMPLETED", "E_GAME_TIMEOUT", "E_RESET_COMPLETE" };
#define DEBUG_PRINT(...)  printf(__VA_ARGS__)
#endif


#define MAX_STATES 20
#define MAX_TRANSITIONS 20
#define MAX_EVENTS_IN_BUFFER 4
#define MAX_EVENTS_IN_BUFFER_MASK (MAX_EVENTS_IN_BUFFER - 1)

state_funcs_t state_funcs[MAX_STATES] = {};
transition_t transitions[MAX_TRANSITIONS] = {};
event_t events[MAX_EVENTS_IN_BUFFER] = {};

static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;

uint8_t numOfStates = 0;
uint8_t numOfTransitions = 0;

/**
 * @brief  Voegt een nieuwe state toe aan de FSM.
 *
 * @param  state  ID van de state die toegevoegd wordt.
 * @param  funcs  {onEntry, onExit, onUpdate } 
 *
 */
void FSM_addState(const state_t state, const state_funcs_t *funcs)
{
    #if DEBUG_FMS
        DEBUG_PRINT("[DEBUG_FSM] State added, Event %s\n",stateNames[state]);
    #endif
    if(numOfStates > MAX_STATES) return;
    memcpy(&state_funcs[state], funcs, sizeof(state_funcs_t));
    numOfStates++;
}
/**
 * @brief  Voegt een nieuwe transition toe aan de beschikbare transitions in de FSM.
 * @param  transition {from, event, to}
 */
void FSM_addTransition(const transition_t *transition)
{
    #if DEBUG_FMS
        DEBUG_PRINT("[DEBUG_FSM] Transition added, From %-30s, Event %-35s, To %-30s\n",stateNames[transitions->from],eventNames[transitions->event],stateNames[transitions->to]);
    #endif
    if(numOfTransitions > MAX_TRANSITIONS) return;
    memcpy(&transitions, transition, sizeof(transition_t));
    numOfTransitions ++;
}

/**
 * @brief  Voegt een event toe aan de nog uit te voeren events in de FSM.
 */
void FSM_addEvent(const event_t event)
{
    uint8_t tmpHead;
    tmpHead = (head + 1) & MAX_EVENTS_IN_BUFFER_MASK;

    if(tmpHead == tail) return;

    events[tmpHead] = event;
    head = tmpHead;
}

/**
 * @brief  Haalt het volgende event uit de event-buffer van de FSM.
 * 
 * @return Het volgende event van type event_t uit de buffer.
 *         Indien de buffer leeg is, wordt E_GEEN teruggegeven.
*/
event_t FSM_getEvent(void)
{
    // Sla event-verwerking over als de event-buffer leeg is
    if(tail == head) return E_GEEN; 

    uint8_t tmpTail;
    event_t event;

    tmpTail = (tail + 1) & MAX_EVENTS_IN_BUFFER_MASK;
    event = events[tmpTail];

    tail = tmpTail;

    return event;
}
/**
 * @brief  Verwerkt een event voor de opgegeven state in de FSM.
 */
state_t FSM_eventHandler(const state_t state, const event_t event)
{
    state_t nextState = state;
    for(uint8_t i = 0; i <= numOfTransitions; ++i)
    {
        if(transitions[i].from == state)
        {
            if(transitions[i].event == event)
            {
                if(state_funcs[transitions[i].from].onExit != NULL)
                {
                    state_funcs[transitions[i].from].onExit();
                }
                nextState = transitions[i].to; 
                if(state_funcs[transitions[i].to].onEntry != NULL)
                {
                    state_funcs[transitions[i].to].onEntry();
                }
            }
        }
    }
    return nextState;
}

/**
 * @brief  Verwerkt events en voert de actieve state van de FSM uit.
 *
 * @note  Deze functie verwerkt slechts één event per aanroep en voert de
 *        onUpdate callback van de huidige state uit. Om de FSM continu te
 *        laten werken, moet deze functie herhaaldelijk in een while-loop
 *        in de main functie worden aangeroepen.
 */
void FSM_runStateMachine(void)
{
    static event_t event;
    static state_t state = S_INIT;

    if(head != tail)
    {
        event = FSM_getEvent();
        state = FSM_eventHandler(state, event);

        #if DEBUG_FMS
            DEBUG_PRINT("[DEBUG_FSM] Triggert event %s vanuit state %s\n\n", eventNames[event], stateNames[state]);
        #endif
    }

    if(state_funcs[state].onUpdate != NULL)
    {
        state_funcs[state].onUpdate();
    }
}