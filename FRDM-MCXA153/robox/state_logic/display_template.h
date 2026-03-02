#ifndef DISPLAY_TEMPLATE_H
#define DISPLAY_TEMPLATE_H

#define MAX_TEMPLATES 10
#define MAX_CHAR_PER_TEMP 10

typedef enum
{
    NON,
    ONE,
    TWO
}displayTemplate_t;

const char displayTemplates[MAX_TEMPLATES][MAX_CHAR_PER_TEMP] =
{
    "Hee",
    "Hoe"
};

#endif