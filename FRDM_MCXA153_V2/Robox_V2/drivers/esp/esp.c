#include "esp.h"
#include "lupart1.h"
bool dataTransfereBuzy = false;

#define START_BIT 0
#define START_DATA_BIT 1
#define STOP_BIT 2

#define REQWEST_DATA_BIT 3
#define START_GET_DATA_BIT 4
#define STOP_GET_DATA_BIT 5

void esp_init(void)
{

}

static inline void send(const int8_t data)
{
    lpuart1_putchar(data);
}

void esp_sendData(uint8_t rawData)
{
    uint8_t *data = (uint8_t *)&rawData;
    size_t size = sizeof(rawData);

    lpuart1_putchar(START_BIT);
    for (size_t i = 0; i < size; i++)
    {
        lpuart1_putchar(data[i]);
    }  
    //Send start bit,
    //for chukns data
        //send data start bit
        //Send data
    
    //Send stopBit
    //wait for confermation 
}

void esp_getData()
{
    //Send reqwest bit
    //wait for start bit
        //wait for data start bit
        //wait for data
    
    //wait for stopbit
}

void esp_update()
{
    if(!dataTransfereBuzy) return;

}