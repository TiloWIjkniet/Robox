#ifndef DISPLAY_TEMPLATE_H
#define DISPLAY_TEMPLATE_H

#define MAX_TEMPLATES 20


#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    IDLE_D,
    D_DEV_PAGE,
    START_D,
    KAMER_D,
    ANTWOORD_D,
    GOED_S_D,
    GOED_D,
    GOED_C_D,
    FOUD_D,
    VOLGENDE_D,
    LAATSTE_D,
    TIJD_D,
    GEHAALT_D,
    RESET_D,
    D_ERROR_RECEIVE_SETTINGS
}displayTemplate_t;

extern  const char *displayTemplates[MAX_TEMPLATES];
#endif