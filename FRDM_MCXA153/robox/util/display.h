#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>

struct hd44780_cmd {
    uint8_t data;
    uint8_t rs;
};

#define HD44780_CMD_BUFF_SIZE 256

void hd44780_init();
int hd44780_writeb(char val);
int hd44780_update();
int setCursor(uint8_t x, uint8_t y);
int cmd_display_clear();
void quickPrint(char *str);
uint32_t getBufferedCmds();
#endif
