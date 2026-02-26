#ifndef FSM_H
#define FSM_H

#include "states.h"
#include "events.h"

typedef struct
{
    void (*onEntry)(void);
    void (*onExit)(void);
    void (*onUpdate)(void);
}state_funcs_t;

typedef struct
{
    state_t from;
    event_t event;
    state_t to;

}transition_t;

void FSM_config(void);
void FSM_runStateMachine(void);
void FSM_addEvent(const event_t event);
void FSM_addState(const state_t state, const state_funcs_t *funcs);

void FSM_addTransition(const transition_t *transition);
state_t FSM_eventHandler(const state_t state, const event_t event);




#endif