#include <stdio.h>
#include <string.h>

#include "lpuart2.h"
#include <stdlib.h>
#include <stdbool.h>
#include "time_millis.h"


char beconIp[50];

#define LINE_BUFFER_SIZE 128
#define BEACON_TIMEOUT 20000
char line_buffer[LINE_BUFFER_SIZE];
uint32_t line_index = 0;
uint32_t lastReseaftMasige = 0;
bool scanInProgress = false;



typedef struct
{
    uint32_t lastSeen;
    char beaconIp[11];
    uint8_t beconStrengt;   
}beacon_t;

beacon_t becons[20];
uint8_t beconIndex = 0;



void sentDataToHM10(const char *mesag)
{
      
    for (uint8_t i = 0; i < 10; i++)
    {
        lpuart2_putchar(mesag[i]);
    }
    lpuart2_putchar('\r');
    lpuart2_putchar('\n');
}

void askForBeacons()
{
    sentDataToHM10("AT+DISA?"); // AT+DISI?
    
}

void HM10_init()
{
    lpuart2_init(9600);
    sentDataToHM10("AT+BAUD4");
    lpuart2_init(115200);

    uint32_t start = millis();
    while((millis() - start) < 100);  // wacht ~100 ms
    
    sentDataToHM10("AT+ROLE1"); // master mode
    sentDataToHM10("AT+IMME0"); // scan gelijk 

    sentDataToHM10("AT+POWE3"); // zet power hooger dus sneler detection en reaction
    sentDataToHM10("AT+IBEA1"); // zorgt er voor dat je allen iBcons ziet 

    
    askForBeacons();
}

void getBeconData()
{
        while(lpuart2_rxcnt() > 0)
        {
            uint8_t data = lpuart2_getchar();
            if (line_index < LINE_BUFFER_SIZE - 1)
            {
                line_buffer[line_index++] = data;
                lastReseaftMasige = millis();
            }

            if (data == '\n')
            {
                line_buffer[line_index] = '\0';
                
                if (strstr(line_buffer, "4C000215") != NULL)
                {
                    char strength[4];
                    strength[0] = line_buffer[75];
                    strength[1] = line_buffer[76];
                    strength[2] = line_buffer[77];
                    strength[3] = '\0';


                    char beaconIp[11] = "          ";
                    for(int i = 0; i < 8; i++)
                    {
                        beaconIp[i] = line_buffer[50+ i];
                    }
                    beaconIp[8] = '\0';

                    bool found = false;
                    for (uint8_t i = 0; i < beconIndex; i++)
                    {
                        if(strcmp(becons[i].beaconIp, beaconIp) == 0)
                        {
                            becons[i].beconStrengt = (uint8_t)atoi(strength);
                            becons[i].lastSeen = millis();
                            found = true;
                            // printf("ip: %s    Strengt:", becons[i].beaconIp);
                            //  printf("%d\n", becons[i].beconStrengt);
                            break;
                        }
                    }

                    if (!found)
                    {
                        strcpy(becons[beconIndex].beaconIp, beaconIp);
                        becons[beconIndex].beconStrengt = (uint8_t)atoi(strength);
                        becons[beconIndex].lastSeen = millis();
                        // printf("found: %s    Strengt:", becons[beconIndex].beaconIp);
                        // printf("%d\n", becons[beconIndex].beconStrengt);
                        beconIndex++;
                    }
                    
                }

                if (strstr(line_buffer, "OK+DISCE") != NULL)
                {
                    scanInProgress = false;
                }
                line_index = 0;
            }
        }
}


void updateHM10()
{
    
   
    if(millis() - lastReseaftMasige > 1000) scanInProgress = false; // meschein sneller mis langzaamer
    if(!scanInProgress) 
    {
        lastReseaftMasige = millis();
        scanInProgress = true;
        askForBeacons();
    }


    getBeconData();

    for (int i = 0; i < beconIndex; i++)
    {
        if (millis() - becons[i].lastSeen > BEACON_TIMEOUT)
        {
            for (int j = i + 1; j < beconIndex; j++)
            {
                becons[j - 1] = becons[j];
            }
            
            beconIndex--;
            i--; 
        }
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
    if(lowestBeaconIndex == -1) 
    {
        strcpy(beconIp, " ");
    }
    else
    {
        strcpy(beconIp, becons[lowestBeaconIndex].beaconIp);
    }

    printf("Lowest: %s\n", beconIp);
}