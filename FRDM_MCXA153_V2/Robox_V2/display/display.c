#include <stdbool.h>

#include "display.h"
#include "time.h"

#define MAX_ITEMS_IN_DISPLAY_QUEUE 2
#define MAX_ITEMS_IN_DISPLAY_QUEUE_MASK (MAX_ITEMS_IN_DISPLAY_QUEUE - 1)

typedef enum 
{
    DISPLAY_STATE_QUEUED,
    DISPLAY_STATE_ACTIVE,
    DISPLAY_STATE_DONE
} displayState_t;

typedef struct 
{
    displayTemplate_t displayTemplate;
    uint32_t duration_ms;
    uint32_t displayStart_ms;
    displayState_t state;
    
}displayQueueItem_t;

static displayQueueItem_t displayQueue[MAX_ITEMS_IN_DISPLAY_QUEUE] = {};

static uint8_t head = 0;
static uint8_t tail = 0;

static displayTemplate_t activeDisplayTemplate = D_NON;


bool display_queueContains(const displayTemplate_t displayTemplate)
{
    uint8_t i = tail;
    while(i != head)
    {
        if(displayQueue[i].displayTemplate == displayTemplate) return true;
        i = (i + 1) & MAX_ITEMS_IN_DISPLAY_QUEUE_MASK;
    }
    
    return false;
}


void display_updateQueue(void)
{
    if(head == tail) return;

    uint32_t now = millis();
    displayQueueItem_t *item = &displayQueue[tail];

    if(item->state == DISPLAY_STATE_QUEUED)
    {
        activeDisplayTemplate = item->displayTemplate;
        item->displayStart_ms = now;
        item->state = DISPLAY_STATE_ACTIVE;
        display_print(activeDisplayTemplate);

    }

    if(now - item->displayStart_ms < item->duration_ms) return;
    item->state = DISPLAY_STATE_DONE;
    tail = (tail + 1) & MAX_ITEMS_IN_DISPLAY_QUEUE_MASK;
}


int display_forceTemplate(const displayTemplate_t displayTemplate, const uint32_t duration_ms)
{
    if(activeDisplayTemplate == displayTemplate) return -1; 
    
    tail = 0;
    head = 1;
    
    displayQueue[tail].displayTemplate   = displayTemplate;
    displayQueue[tail].duration_ms       = duration_ms;
    displayQueue[tail].displayStart_ms   = 0;
    displayQueue[tail].state             = DISPLAY_STATE_QUEUED;


    return 0;
}

int display_queueTemplate(const displayTemplate_t displayTemplate, const uint32_t duration_ms)
{
    if(activeDisplayTemplate == displayTemplate) return -1;
    if(queueContainsDisplay(displayTemplate)) return -1;

    uint8_t tmpHead = (head + 1) & MAX_ITEMS_IN_DISPLAY_QUEUE_MASK;
    if(tmpHead == tail) return -1;

    displayQueue[tmpHead].displayTemplate   = displayTemplate;
    displayQueue[tmpHead].duration_ms       = duration_ms;
    displayQueue[tmpHead].displayStart_ms   = 0;
    displayQueue[tmpHead].state             = DISPLAY_STATE_QUEUED;

    head = tmpHead;

    return 0;
}

void display_print(const displayTemplate_t displayTemplate)
{

}

