#ifndef KEY_PAD_H
#define KEY_PAD_H

#include <stdbool.h>
#include <stdint.h>
#include "game_data.h"
void updateInputBuffer(void);
void emptyInputBuffer(void);
void keyPad_init(void);

extern char answerBuffer[MAX_CHAR_IN_STRING];
extern bool hasNewAnswer;

#endif