#include "hm10.h"
#include "time.h"
#include "lpuart2.h"
#include <limits.h>
#include "delay.h"

#define MAX_BEACONS 5
#define BEACON_STRENGTH_CHAR_LEN 4
#define BEACON_IP_CHAR_LEN 8

#define SEND_DATA_LEN 10
#define STRENGTH_START_INDEX 75
#define IP_START_INDEX 50

#define LINE_BUFFER_SIZE 100
 
#define BEACON_NULL 0
#define SEND_DELAY (1 * SEC_TO_MS)
#define BEACON_TIME_OUT_MS (1 * MIN_TO_MS)
typedef struct 
{
    char ip[BEACON_IP_CHAR_LEN];
    uint8_t strength;
    uint32_t lastStableTime;
}beacon_t;

beacon_t beacons[MAX_BEACONS] = {};
uint8_t beacons_head = 0;

static inline void HM10_sentData(const char *mesag)
{
    for (uint8_t i = 0; mesag[i] != '\0' && i < UINT8_MAX; i++)
    {
        lpuart2_putchar(mesag[i]);
    }
    lpuart2_putchar('\r');
    lpuart2_putchar('\n');
}

void HM10_init(void)
{
    lpuart2_init(9600);
    delay_ms(10);
    HM10_sentData("AT+BAUD4");
    delay_ms(10);
    lpuart2_init(115200);
    delay_ms(10);
    HM10_sentData("AT+ROLE1"); 
    HM10_sentData("AT+IMME0"); 
    HM10_sentData("AT+IBEA1"); 
}

static void HM10_pop(uint8_t index)
{
    if(beacons_head == 0 || index >= beacons_head) return;
    beacons_head--;
    beacons[index] = beacons[beacons_head]; 
}

static void HM10_removeExpiredBeacons(void)
{
    uint32_t now = millis();
    for (uint8_t i = 0; i < beacons_head;)
    {
        if(now - beacons[i].lastStableTime > BEACON_TIME_OUT_MS)
        {
            HM10_pop(i);
        }
        else
        {
            i++;
        }
    }
}

static uint8_t HM10_getStrongestBeaconIndex(void)
{
    uint8_t weakestIndex = 0;
    for(uint8_t i = 0; i < beacons_head; i ++)
    {
       if(beacons[i].strength > beacons[weakestIndex].strength) 
       {
            weakestIndex = i;     
       }
    }
    return weakestIndex;
}

static uint8_t HM10_getWeakestBeaconIndex(void)
{
    uint8_t strongestIndex = 99;
    for(uint8_t i = 0; i < beacons_head; i ++)
    {
       if(beacons[i].strength < beacons[strongestIndex].strength) 
       {
            strongestIndex = i;     
       }
    }
    return strongestIndex;
}

static int8_t HM10_getBeaconIndex(char *beaconIp)
{
    for (uint8_t i = 0; i < beacons_head; i++)
    {
        if(strcmp(beacons[i].ip, beaconIp) == 0 ) return i;
    }
    return -1;
    
}

static void HM10_extractIp(const char *line, char *out)
{
    memcpy(out, &line[IP_START_INDEX], BEACON_IP_CHAR_LEN - 1);
    out[BEACON_IP_CHAR_LEN - 1] = '\0';
}

static uint8_t HM10_extractStrength(const char *line)
{
    uint8_t val = 0;

    for (uint8_t i = 0; i < BEACON_STRENGTH_CHAR_LEN - 1; i++)
    {
        char c = line[STRENGTH_START_INDEX + i];
        if (c < '0' || c > '9') break;

        val = val * 10 + (c - '0');
    }

    return val;
}

static void HM10_updateBeacon(char *beaconIp, uint8_t strength)
{
    uint32_t now = millis();
    int8_t beaconIndex = HM10_getBeaconIndex(beaconIp);
    if(beaconIndex >= 0)
    {
        beacons[beaconIndex].strength = strength;
        beacons[beaconIndex].lastStableTime = now;
        return;
    }

    
    if(beacons_head >= MAX_BEACONS) 
    {
        uint8_t weakestBeaconIndex =  HM10_getWeakestBeaconIndex();
        HM10_pop(weakestBeaconIndex);
    }
  
    uint8_t beacons_headLastVlaue = beacons_head;
    beacons_head ++;
    
    memcpy(beacons[beacons_headLastVlaue].ip, beaconIp, BEACON_IP_CHAR_LEN);
    beacons[beacons_headLastVlaue].strength = strength;
    beacons[beacons_headLastVlaue].lastStableTime = now; 
}

static void HM10_checkResponse(void)
{
    static char line_buffer[LINE_BUFFER_SIZE] = {0};
    static uint8_t line_index = 0;

    while(lpuart2_rxcnt() > 0)
    {
        uint8_t data = lpuart2_getchar();
        if (line_index < LINE_BUFFER_SIZE - 1)
        {
            line_buffer[line_index++] = data;
        }

        if (data != '\n') continue;
        
        line_buffer[line_index] = '\0';
        line_index = 0;
        if (strstr(line_buffer, "4C000215") == 0) continue;
        
        uint8_t strength = HM10_extractStrength(line_buffer);

        char beaconIp[BEACON_IP_CHAR_LEN] = {0};
        HM10_extractIp(beaconIp, line_buffer);

        HM10_updateBeacon(beaconIp, strength);
    }
}



void HM10_getLowestBeaconIp(char *ip)
{
    uint8_t strongestBeaconIndex = HM10_getStrongestBeaconIndex();
    memcpy(ip, beacons[strongestBeaconIndex].ip, BEACON_IP_CHAR_LEN);
}

void HM10_update(void)
{
    static uint32_t lastSendTime = 0;

    HM10_checkResponse();

    uint32_t now = millis();
    if(now - lastSendTime < SEND_DELAY) return;
    lastSendTime = now;
    HM10_sentData("AT+DISI?");
    HM10_removeExpiredBeacons();

}