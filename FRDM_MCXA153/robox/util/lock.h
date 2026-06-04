#ifndef LOCK_H
#define LOCK_H

#include <stdint.h>
#include <stdbool.h>

void lock_init(void);
void openLock(void);
void lockUpdate(void);
bool isLockOpen();
void closeLock();
#endif