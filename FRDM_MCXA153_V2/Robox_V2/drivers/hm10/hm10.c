#include "hm10.h"
#include "time.h"
#include "lpuart2.h"
#include <limits.h>
#include "delay.h"


//NOTE: To be optimize
#define MAX_BEACONS 5
#define LINE_BUFFER_SIZE 100

#define SEND_DELAY (1 * SEC_TO_MS)
#define BEACON_TIME_OUT_MS (1 * MIN_TO_MS)



#define BEACON_STRENGTH_CHAR_LEN 4
#define BEACON_IP_CHAR_LEN 8

#define SEND_DATA_LEN 10
#define STRENGTH_START_INDEX 75
#define IP_START_INDEX 50

typedef struct 
{
    char ip[BEACON_IP_CHAR_LEN];
    uint8_t strength;
    uint32_t lastStableTime;
}beacon_t;

beacon_t beacons[MAX_BEACONS] = {};
uint8_t beacons_len = 0;

static inline void HM10_sendData(const char *message)
{
    const char *p = message;
    while(*p)
    {
        lpuart2_putchar(*p++);
    }
    
    lpuart2_putchar('\r');
    lpuart2_putchar('\n');
}

void HM10_init(void)
{
    lpuart2_init(9600);
    delay_ms(10);
    HM10_sendData("AT+BAUD4");
    delay_ms(10);
    lpuart2_init(115200);
    delay_ms(10);
    HM10_sendData("AT+ROLE1"); 
    HM10_sendData("AT+IMME0"); 
    HM10_sendData("AT+IBEA1"); 
}

static inline void HM10_pop(const uint8_t index)
{
    if(index >= beacons_len) return;
    beacons[index] = beacons[beacons_len]; 
}

static void HM10_removeExpiredBeacons(void)
{
    uint32_t now = millis();
    beacon_t *b = beacons;
    for (uint8_t i = 0; i < beacons_len;)
    {
        if(now - b[i].lastStableTime > BEACON_TIME_OUT_MS)
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
    uint8_t head = beacons_len;
    beacon_t *b = beacons;

    if(head == 0) return;
    uint8_t  strongestIndex = b[0].strength;
   
    for(uint8_t i = 1; i < head; i ++)
    {
       if(b[i].strength > b[strongestIndex].strength) 
       {
            strongestIndex = i;     
       }
    }
    return strongestIndex;
}

static uint8_t HM10_getWeakestBeaconIndex(void)
{
    uint8_t head = beacons_len;
    beacon_t *b = beacons;

    if(head == 0) return;
    uint8_t  strongestIndex = b[0].strength;

    for(uint8_t i = 1; i < head; i ++)
    {
       if(b[i].strength < b[weakestIndex].strength) 
       {
            weakestIndex = i;     
       }
    }
    return weakestIndex;
}

static int8_t HM10_getBeaconIndex(char *beaconIp)
{
    for (uint8_t i = 0; i < beacons_len; i++)
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
    const char *p = line + STRENGTH_START_INDEX;
    for (uint8_t i = 0; i < BEACON_STRENGTH_CHAR_LEN - 1; i++)
    {
        char c = *p++;
        if (c < '0' || c > '9') break;

        val = val * 10 + (c - '0');
    }
    return val;
}

static void HM10_updateBeacon(char *beaconIp, const uint8_t strength)
{
    uint32_t now = millis();
    int8_t beaconIndex = HM10_getBeaconIndex(beaconIp);
    if(beaconIndex >= 0)
    {
        beacons[beaconIndex].strength = strength;
        beacons[beaconIndex].lastStableTime = now;
        return;
    }

    if(beacons_len >= MAX_BEACONS) 
    {
        uint8_t weakestBeaconIndex =  HM10_getWeakestBeaconIndex();
        HM10_pop(weakestBeaconIndex);
    }
  
    beacons_len ++;
    
    memcpy(beaconsb[beacons_len].ip, beaconIp, BEACON_IP_CHAR_LEN);
    beacons[beacons_len].strength = strength;
    beaconsb[beacons_len].lastStableTime = now; 
}

bool HM10_isIBeacon(char *buffer, char *data)
{
    //function vervangen door op de goede bytes te kijken inplaat van door ze allemaal heen te loopen
    return (strstr(buffer, data) == 0);
}   

static void HM10_checkResponse(void)
{
    static char line_buffer[LINE_BUFFER_SIZE] = {0};
    static uint8_t line_index = 0;
    uint8_t cnt = lpuart2_rxcnt();
    while(cnt--)
    {
        uint8_t data = lpuart2_getchar();

        
        if (line_index >= LINE_BUFFER_SIZE - 1) line_index = 0;
        line_buffer[line_index++] = data;

        if (data != '\n' && data != '\r') continue;
        
        line_buffer[line_index] = '\0';
        line_index = 0;

        if (!HM10_isIBeacon()) continue;
        
        uint8_t strength = HM10_extractStrength(line_buffer);

        char beaconIp[BEACON_IP_CHAR_LEN] = {0};
        HM10_extractIp(beaconIp, line_buffer);

        HM10_updateBeacon(beaconIp, strength);
    }
}

void HM10_getStrongestBeaconIp(const char *ip)
{
    uint8_t strongestBeaconIndex = HM10_getStrongestBeaconIndex();
    memcpy(ip, beacons[strongestBeaconIndex].ip, BEACON_IP_CHAR_LEN);
}

int HM10_Qsort(const beacon_t *a, const beacon_t*b)
{
    
    const beacon_t *ba = a;
    const beacon_t *bb = b;
    return ba->strength - bb->strength;
}

void HM10_getStrongest5Beacons(const char *b, const size_t size)
{
    if(beacons_len <= 0) 
    {
        char *str = "No beacons";
        memcpy(b, "No beacons", sizeof("No beacons"));
        return;
    }
    qsort(beacons, beacons_len, sizeof(beacon_t), HM10_Qsort);

    size_t offset = 0;
    for (uint8_t i = 0; i < 5; i++)
    {
        int written  = sprintf(b, sizeof(b), "%s  %d",beacons[i].ip, beacons[i].strength);

        if(written < 0|| written >= size - offset) return;
        offset += written;
    }
        
}
void HM10_update(void)
{
    static uint32_t lastSendTime = 0;

    HM10_checkResponse();

    uint32_t now = millis();
    if(now - lastSendTime < SEND_DELAY) return;
    lastSendTime = now;

    HM10_sendData("AT+DISI?");
    HM10_removeExpiredBeacons();

}