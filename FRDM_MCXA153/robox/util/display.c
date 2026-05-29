#include "display.h"
#include "port.h"
#include "time_millis.h"
void hd44780_init()
{
    port_init();
    delay(50);        // wait after power-up

                    // wait after power-up
    write_port(0, 0b00110000);
    delay(5);
    write_port(0, 0b00001111);
    delay(5);
    write_port(0, 0b00000110);
    delay(5);
    write_port(0, 0b00000001);
    delay(5);
}
static const uint8_t DDRAM_addr[] = {
    0x00,
    0x10,
    0x8,
    0x18,
};

static uint8_t cmd_head = 0;
static uint8_t cmd_tail = 0;
struct hd44780_cmd cmd_buff[HD44780_CMD_BUFF_SIZE];
int hd44780_push_cmd(uint8_t rs, uint8_t val)
{
    uint8_t tmp = (cmd_head + 1) & (HD44780_CMD_BUFF_SIZE - 1);
    if (tmp == cmd_tail) {
        return -1;
    }
    cmd_head = tmp;
    cmd_buff[tmp] = (struct hd44780_cmd) { .data = val, .rs = rs};
    return 0;
}

int cmd_display_clear()
{
    return hd44780_push_cmd(0, 1);
}

static int cmd_set_ddram(uint8_t val)
{
    return hd44780_push_cmd(0, 0x80 | (val & 0x7F));
}

int setCursor(uint8_t x, uint8_t y)
{
    if (y > 3 || x > 15) 
    {
        return -1;
    }
    return cmd_set_ddram(DDRAM_addr[y] + x);
}

uint32_t getBufferedCmds()
{
    if (cmd_head >= cmd_tail) 
    {
        return cmd_head - cmd_tail;
    } else 
    {
        return HD44780_CMD_BUFF_SIZE - (cmd_tail - cmd_head);
    }
}

static uint8_t newl_pending = 0;
static uint8_t cursor_y = 0;
static uint8_t cursor_x = 0;
int hd44780_writeb(char val)
{
    if (newl_pending) {
        uint8_t tmp = cursor_y + 1;
        if (cmd_set_ddram(DDRAM_addr[tmp]) == -1) { return -1; };
        cursor_x = 0;
        cursor_y = tmp;
        newl_pending = 0;
    }
    
    if (cursor_y >= 4) {
        if (cmd_display_clear() == -1) { return -1; };
        cursor_x = 0;
        cursor_y = 0;
    }
    if (val == '\n') {
        newl_pending = 1;
    } else if (val == '\b') {
        cursor_x--;
    } else {
        if (hd44780_push_cmd(1, val) == -1) { return -1; };
        cursor_x++;
    }
    if (cursor_x >= 16) {
        newl_pending = 1;
    }
    return 0;
}

void hd44780_update()
{
    uint8_t status = read_port(0);

    if (!(status & 0x80)) {

        if (cmd_head != cmd_tail) 
        {
            uint8_t tmp = (cmd_tail + 1) & (HD44780_CMD_BUFF_SIZE - 1);
            cmd_tail = tmp;
            struct hd44780_cmd* cmd = &cmd_buff[tmp];
            write_port(cmd->rs, cmd->data);
        }
    }
}