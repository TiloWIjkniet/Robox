#include <string.h>

#include "fsm.h"

#define MAX_STATES 20
#define MAX_TRANSITIONS 20
#define MAX_EVENTS_IN_BUFFER 2
#define MAX_EVENTS_IN_BUFFER_MASK (MAX_EVENTS_IN_BUFFER - 1)


state_funcs_t   stateFuncs[MAX_STATES] = {};
transition_t    transitions[MAX_TRANSITIONS] = {};
event_t         events[MAX_EVENTS_IN_BUFFER] = {};


static uint8_t head = 0;
static uint8_t tail = 0;

uint8_t numOfStates = 0;
uint8_t numOfTransitions = 0;

int FSM_addState(const state_t state, const state_funcs_t *funcs)
{
    if(numOfStates > MAX_STATES) return -1;
    memcpy(&stateFuncs[state], funcs, sizeof(state_funcs_t));
    numOfStates++;
    return 0;
}

int FSM_addTransition(const transition_t *transition)
{
    if(numOfTransitions > MAX_TRANSITIONS) return -1;
    memcpy(&transitions[numOfTransitions], transition, sizeof(transition_t));
    numOfTransitions ++;
    return 0;
}

int FSM_addEvent(const event_t event)
{
    uint8_t tmpHead;
    tmpHead = (head + 1) & MAX_EVENTS_IN_BUFFER_MASK;

    if(tmpHead == tail) return -1;
    events[tmpHead] = event;
    head = tmpHead;
    return 0;
}

event_t FSM_getEvent(void)
{
    if(tail == head) return E_NON; 

    uint8_t tmpTail;
    event_t event;

    tmpTail = (tail + 1) & MAX_EVENTS_IN_BUFFER_MASK;
    event = events[tmpTail];

    tail = tmpTail;
    return event;
}

state_t FSM_eventHandler(const state_t state, const event_t event)
{
    state_t nextState = state;
    for(uint8_t i = 0; i < numOfTransitions; ++i)
    {
        if(transitions[i].from == state)
        {
            if(transitions[i].event == event)
            {
                if(stateFuncs[transitions[i].from].onExit != NULL)
                {
                    stateFuncs[transitions[i].from].onExit();
                }

                nextState = transitions[i].to; 

                if(stateFuncs[transitions[i].to].onEntry != NULL)
                {
                    stateFuncs[transitions[i].to].onEntry();
                }
            }
        }
    }
    return nextState;
}

void FSM_update(void)
{
    static event_t event;
    static state_t state = S_INIT;

    if(head != tail)
    {
        event = FSM_getEvent();

        state = FSM_eventHandler(state, event);
    }

    if(stateFuncs[state].onUpdate != NULL)
    {
        stateFuncs[state].onUpdate();
    }
}