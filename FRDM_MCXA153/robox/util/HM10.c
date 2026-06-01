#include <stdio.h>
#include <string.h>

#include "lpuart2.h"
#include <stdlib.h>
#include <stdbool.h>
#include "time_millis.h"
#include "HM10.h"
#include "game_data.h"
char beconIp[MAX_CHAR_IN_STRING];

#define LINE_BUFFER_SIZE 128
#define BEACON_TIMEOUT 20000
#define CASH_SISE 5
#define NOT_CONECTION_PANALTY 5
char line_buffer[LINE_BUFFER_SIZE];
uint32_t line_index = 0;
uint32_t lastReseaftMasige = 0;
bool scanInProgress = false;



typedef struct
{
    uint32_t lastSeen;
    char beaconIp[11];
    int8_t beconStrengt; 
    int8_t cash[CASH_SISE];
}beacon_t;

beacon_t becons[MAX_BEACONS];
uint8_t beconIndex = 0;
uint8_t beconLoaclIndex = 0;

void sentDataToHM10(const char *mesag)
{
   for (uint8_t i = 0; mesag[i] != '\0'; i++)
    {
        lpuart2_putchar(mesag[i]);
    }
}
/**
 * @brief Request beacon scan from HM-10 module.
 *
 * Sends the AT command to start or request a device discovery scan.
 *
 * @note Transmits "AT+DISI?" over UART to the HM-10 module.
 */
static inline void askForBeacons(void)
{
    sentDataToHM10("AT+DISI?\r\n"); // AT+DISI?
}

void HM10_init(void)
{
    // WARN: Heb dit niew toe gevoegt dus werkt mis niet goed
    lpuart2_init(115200);
    delay(1000);
    sentDataToHM10("AT\r\n");
    delay(1000);
    sentDataToHM10("AT+RESET\r\n");
    delay(1000);
    sentDataToHM10("AT+ROLE1\r\n"); // master mode
    delay(1000);
    sentDataToHM10("AT+IMME1\r\n"); // scan gelijk 
    delay(1000);
    sentDataToHM10("AT+POWE3\r\n"); // zet power hooger dus sneler detection en reacti
    delay(1000);
    sentDataToHM10("AT+ROLE?\r\n");
    delay(1000);
   // sentDataToHM10("AT+IBEA1\r\n"); // zorgt er voor dat je allen iBcons ziet 
}


void updateBeacon(char *beaconIp, int strength)
{
    bool found = false;
    for (uint8_t i = 0; i < beconIndex; i++)
    {
        if(strcmp(becons[i].beaconIp, beaconIp) == 0)
        {
            becons[i].cash[beconLoaclIndex] = strength;
            becons[i].lastSeen = millis();
            found = true;
            break;
        }
    }
    //Als nog niet in de lijst, voeg deze dan toe aan de lijst met beacons
    if (!found)
    {
        uint8_t b = 0;
        if(beconIndex >= MAX_BEACONS)
        {
            int8_t lowesStrengt = 0;
            for (uint8_t i = 0; i < beconIndex; i++)
            {
 
                if(becons[i].beconStrengt > lowesStrengt)
                {
                    lowesStrengt = becons[i].beconStrengt;
                    b = i;

                }
                
            }

        }
        else
        {
            b = beconIndex;
            beconIndex++;
        }

        strcpy(becons[b].beaconIp, beaconIp);
        for(uint8_t i = 0; i < CASH_SISE; i ++)
        {
            becons[b].cash[i] = 0;
        }
        becons[b].cash[beconLoaclIndex] = strength;
        becons[b].lastSeen = millis();
        
    }
}

void calculateStrenkt()
{
    beconLoaclIndex++;
    if(beconLoaclIndex >= CASH_SISE) beconLoaclIndex = 0;

    for (uint8_t i = 0; i < beconIndex; i++)
    {
        uint32_t sum = 0;
        uint8_t count = 0;
        for (uint8_t y = 0; y < CASH_SISE; y++)
        {
            if(becons[i].cash[y] != 0)
            {
                count ++;
                sum += becons[i].cash[y];
            }
        }
        uint8_t average = sum == 0 ? 100 : sum/count;
        becons[i].beconStrengt = average + NOT_CONECTION_PANALTY * (CASH_SISE - count);
    } 
    
}
/**
 * @brief Read and process beacon data from UART.
 *
 * Reads incoming UART data line-by-line and parses iBeacon frames.
 * Extracts beacon ID and signal strength, then updates the beacon list.
 */
void getBeconData(void)
{
        while(lpuart2_rxcnt() > 0)
        {
            uint8_t data = lpuart2_getchar();
            if (line_index < LINE_BUFFER_SIZE - 1)
            {
                line_buffer[line_index++] = data;
                
            }

            if (data == '\n')
            {
                //Er is een volledige regel ontvangen, deze verwerken
                lastReseaftMasige = millis(); // reset timeout timer

                line_buffer[line_index] = '\0';
              
                if (strstr(line_buffer, "4C000215") != NULL)
                {   
                    //Komt overeen met iBeacon formaat, dus beacon data uitlezen
                    
                    //Extract strenght en beaconIp uit de regel
                    char strength[4];
                    strength[0] = line_buffer[75];
                    strength[1] = line_buffer[76];
                    strength[2] = line_buffer[77];
                    strength[3] = '\0';

                    //Ibecon aderes 
                    char beaconIp[11] = "          ";
                    for(int i = 0; i < 8; i++)
                    {
                        beaconIp[i] = line_buffer[50+ i];
                    }
                    beaconIp[8] = '\0';

                    //Controleer of deze beacon al in de lijst staat, zo ja update dan de sterkte en last seen tijd
                    updateBeacon(beaconIp, (uint8_t)atoi(strength));
                    
                }
                //Controleer of regel het einde van een scan is
                if (strstr(line_buffer, "OK+DISCE") != NULL)
                {
                    scanInProgress = false;
                }
                line_index = 0;
            }
        }
}

static inline int compare(const void *a, const void *b)
{
    beacon_t *ia = (beacon_t *)a;
    beacon_t *ib = (beacon_t *)b;

    return ib->beconStrengt - ia->beconStrengt; 
}

/**
 * @brief Get formatted list of strongest beacons.
 *
 * Sorts the beacon list by signal strength and writes up to
 * MAX_BEACONS_IN_LIST strongest beacons into the provided buffer.
 *
 * Output format per beacon:
 * "Beacon: <IP>, Strength: <value>\n"
 *
 * If no beacons are available, writes "No beacons found".
 *
 * @param[out] pLowestBeconsString Output buffer for formatted string.
 * @param[in]  size                Size of the output buffer.
 */
void getLowestBecons(char *pLowestBeconsString, uint16_t size)
{
    qsort(becons, beconIndex, sizeof(beacon_t), compare);
    uint16_t offset = 0;
    uint8_t beconsInLowest = beconIndex < MAX_BEACONS_IN_LIST? beconIndex : MAX_BEACONS_IN_LIST;
    for(uint8_t i = 0; i < beconsInLowest; i++)
    {
        offset += snprintf(pLowestBeconsString + offset, size - offset,"Beacon: %s, Strength: %d\n", becons[i].beaconIp, becons[i].beconStrengt);
    }
    if(beconIndex <= 0)
    {
        snprintf(pLowestBeconsString, size, "No beacons found");
    }
}

/**
 * @brief Remove expired beacons from the list.
 *
 * Iterates through the beacon array and removes entries that
 * have not been seen within BEACON_TIMEOUT. Remaining elements
 * are shifted to keep the array contiguous.
 *
 */
void removeOldBecons(void)
{
    uint32_t now = millis();
    for (int i = 0; i < beconIndex; i++)
    {
        if (now - becons[i].lastSeen > BEACON_TIMEOUT || becons[i].beconStrengt >= 90)
        {
            for (int j = i + 1; j < beconIndex; j++)
            {
                becons[j - 1] = becons[j];
            }
            
            beconIndex--;
            i--; 
        }
    }
}
/**
 * @brief Get IP of the strongest beacon.
 *
 * Searches the beacon list for the beacon with the highest signal strength
 * (lowest strength value) and copies its IP into the provided buffer.
 * If no beacons are available, an empty string is returned.
 *
 * @param[out] pBeconIp Buffer where the beacon IP will be stored.
 */
void getStrongestBeconIp(char *pBeconIp)
{

    if(beconIndex <= 0) 
    {
        strcpy(pBeconIp, " ");
        return;
    }

    uint16_t lowesStrenght = 1000;
    int8_t lowestBeaconIndex = -1;
    for (uint8_t i = 0; i < beconIndex; i++)
    {
        if(becons[i].beconStrengt < lowesStrenght)
        {
            lowesStrenght = becons[i].beconStrengt;
            lowestBeaconIndex = i;

        }
    }
    strcpy(pBeconIp, becons[lowestBeaconIndex].beaconIp);

}
void updateHM10(void)
{
   //Controleer of er een scan bezig is, als dit het geval is, dan de beacons data uitlezen en verwerken
   //Als er al een tijd geen data meer is ontvangen, dan aannemen dat de scan klaar is en de volgende scan starten
    if(millis() - lastReseaftMasige > 1000) scanInProgress = false; 
    if(scanInProgress) 
    {
        getBeconData();
        return;
    }
    lastReseaftMasige = millis();
    scanInProgress = true;
    askForBeacons();
    
    calculateStrenkt();
    removeOldBecons();

    getStrongestBeconIp(beconIp);

    char lowestBeconsString[200];
    getLowestBecons(lowestBeconsString, sizeof(lowestBeconsString));
    // printf("\nLowest beacons:\n");
    // printf(lowestBeconsString);

}

bool isInCorrectRoom(const char *beconIp)
{
    for (uint8_t i = 0; i < beconIndex; i++)
    {
        if(strcmp(beconIp, becons[i].beaconIp) == 0) 
        {
            if(becons[i].beconStrengt <= 90) return true;
            return false;
        }
    }
    return false;
}  