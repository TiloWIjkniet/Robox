#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

#define ST7920_CMD_BUFF_SIZE 128

struct st9720_cmd {
    uint8_t data;
    uint8_t rs;
};

int st7920_push_cmd(uint8_t rs, uint8_t val);
int cmd_function_set(bool RE);
int cmd_display_control(bool display, bool cursor, bool blink);
int cmd_entry_mode_set(bool ID, bool S);
int cmd_display_clear();
int cmd_set_ddram(uint8_t val);
int st7920_set_cursor(uint8_t row, uint8_t col);

void st7920_init();
int st7920_writeb(uint8_t val);
void st7920_update();


#endif
