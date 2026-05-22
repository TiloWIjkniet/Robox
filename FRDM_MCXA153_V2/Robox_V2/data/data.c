#include "data.h"

globalSettings_t globalSettings =
{
    WRONG_ANSWER_MINUS_5MIN_STOP,
    10,
    AUDIO_OFF,
    LANGUAGE_ENGLISH,
    NOT_CENSORED
};
runData_t runData;
roomSettings_t roomsSettings[MAX_ROOMS] = 
{
    {{0,1}, "0000", {"0000"}, NON_C, KEY_S, "Room 1"},
    {{1,0}, "1111", {"1111"}, NON_C, SWITCH_S, "Room 2"},
    {{1,1}, "2222", {"2222"}, NON_C, NON_S, "Room 3"}
};