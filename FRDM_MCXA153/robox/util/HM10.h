#ifndef HM10_H
#define HM10_H


#define MAX_BEACONS 20
#define MAX_BEACONS_IN_LIST 5
#define MAX_CHAR_IN_STRING 20

void updateHM10(void);
void HM10_init(void);
void getLowestBecons(char *pLowestBeconsString, uint16_t size);
extern char beconIp[MAX_CHAR_IN_STRING];
#endif