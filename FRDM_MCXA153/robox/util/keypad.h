#ifndef KEY_PAD_H
#define KEY_PAD_H

#include <stdbool.h>
#include <stdint.h>
#define CHAR_IN_STRING 50
void updateInputBuffer(void);
void emptyInputBuffer(void);
void keyPad_init(void);

extern char answerBuffer[CHAR_IN_STRING];
extern bool hasNewAnswer;

#endif