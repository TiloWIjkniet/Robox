#include "display.h"
#include <stddef.h>
#include <board.h>
#include "time_millis.h"
#define RS_SHIFT 8
#define EN_MASK (1 << 9)

struct st9720_cmd disp_cmd_buff[ST7920_CMD_BUFF_SIZE];
uint8_t disp_cmd_tail;
uint8_t disp_cmd_head;
uint8_t disp_cursor_pos;
uint8_t disp_mode;
static const uint8_t DDRAM_addr[4] = {
    0x00,
    0x10,
    0x8,
    0x18
};

int st7920_set_cursor(uint8_t row, uint8_t col)
{
    uint8_t newCursor = row * 16 + col;
    if(newCursor > 4 * 16) return -1;
    
    disp_cursor_pos = newCursor;
    if (cmd_set_ddram(DDRAM_addr[(disp_cursor_pos >> 4)]) == -1) { return -1; };
    return 0;
}

static inline void tiny_delay(int loops)
{
    for (volatile int i = 0; i < loops; i++) {
        __NOP();
    }
}

static inline void ms_delay(uint32_t ms)
{
    uint32_t start = millis();
    while (millis() - start < ms) 
    {
        __NOP();
    }
}

static inline void write_port(uint8_t rs, uint8_t val)
{
    GPIO1->PCOR = 0x3FF; //clear the parralel bus
    GPIO1->PSOR = (rs << RS_SHIFT) | val;
    tiny_delay(1);
    GPIO1->PSOR = EN_MASK;
    tiny_delay(1);
    GPIO1->PCOR = EN_MASK;
    tiny_delay(1);
    GPIO1->PCOR = 0x3FF;
}

int st7920_push_cmd(uint8_t rs, uint8_t val)
{
    uint8_t tmp = (disp_cmd_head + 1) & (ST7920_CMD_BUFF_SIZE - 1);
    if (tmp == disp_cmd_tail) {
        return -1;
    }
    disp_cmd_head = tmp;
    disp_cmd_buff[tmp] = (struct st9720_cmd) { .data = val, .rs = rs};
    return 0;
}



int cmd_function_set(bool RE)
{
    return st7920_push_cmd(0, (0b11 << 4) | (RE << 2));
}

int cmd_display_control(bool display, bool cursor, bool blink)
{
    return st7920_push_cmd(0, (1 << 3) | (display << 2) | (cursor << 1) | blink);
}

int cmd_entry_mode_set(bool ID, bool S)
{
    return st7920_push_cmd(0, (1 << 2) | (ID << 1) | S);
}

int cmd_display_clear()
{
    return st7920_push_cmd(0, 1);
}

int cmd_set_ddram(uint8_t val)
{
    return st7920_push_cmd(0, 0x80 | (val & 0x7F));
}

void st7920_init()
{
    MRCC0->MRCC_GLB_CC1 |= MRCC_MRCC_GLB_CC1_GPIO1(1);
    MRCC0->MRCC_GLB_CC0 |= MRCC_MRCC_GLB_CC0_PORT1(1);
    MRCC0->MRCC_GLB_RST1 |= MRCC_MRCC_GLB_RST1_GPIO1(1);
    MRCC0->MRCC_GLB_RST0 |= MRCC_MRCC_GLB_RST0_PORT1(1);
    for (int i = 0; i < 10; i++) {
        PORT1->PCR[i] = 0x00008000;
        GPIO1->PCOR = (1<<i);
        
        GPIO1->PDDR |= (1<<i);
    }
    
    ms_delay(20);
    write_port(0, 0b00110000);
    ms_delay(5);
    write_port(0, 0b00110000);
    ms_delay(5);
    write_port(0, 0b00001111);
    ms_delay(5);
    write_port(0, 0b00000110);
    ms_delay(5);
    write_port(0, 0b00000001);
    ms_delay(10);
}

int st7920_writeb(uint8_t val)
{
    if (disp_mode & (1 << 0)) {
        if (cmd_set_ddram(DDRAM_addr[(disp_cursor_pos >> 4)]) == -1) { return -1; };
        disp_mode &= ~(1 << 0);
    }

    if (disp_cursor_pos == 4*16) {
        if (cmd_display_clear() == -1) { return -1; };
        disp_cursor_pos = 0;
    }
    if (val == '\n') {
        disp_cursor_pos &= 0xF0;
        disp_cursor_pos += 16;
    } else if (val == '\b') {
        disp_cursor_pos--;
    } else {
        if (st7920_push_cmd(1, val) == -1) { return -1; };
        disp_cursor_pos++;
    }
    if ((disp_cursor_pos & 0xF) == 0) {
        disp_mode |= (1 << 0);
        return -1;
    }
    return 0;
}

void st7920_update()
{
    static uint32_t lastDisplayUpdate = 0;
    uint32_t now = millis();
    if(now - lastDisplayUpdate < 1)  return; 
    lastDisplayUpdate = now;
    
    if (disp_cmd_head != disp_cmd_tail) 
    {
        uint8_t tmp = (disp_cmd_tail + 1) & (ST7920_CMD_BUFF_SIZE - 1);
        disp_cmd_tail = tmp;
        struct st9720_cmd* cmd = &disp_cmd_buff[tmp];
        write_port(cmd->rs, cmd->data);
    }    
}









