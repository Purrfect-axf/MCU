#ifndef INC_I2C_LCD_H_
#define INC_I2C_LCD_H_

#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <string.h>

void lcd_init(void);
void lcd_send_cmd(char cmd);
void lcd_send_data(char data);
void lcd_send_string(char *str);
void lcd_clear(void);
void lcd_put_cur(int row, int col);
void lcd_backlight_on(void);
void lcd_backlight_off(void);

/* เก็บสถานะข้อความเลื่อน */
typedef struct
{
    uint16_t offset;
    int8_t direction;
    uint32_t lastTick;
} LCD_ScrollState;

/* เรียกซ้ำในลูป โดยไม่หยุดรอ จึงอ่านปุ่มได้ตลอด */
static inline void lcd_scroll_bounce(
    LCD_ScrollState *state,
    const char *text,
    uint8_t row,
    uint32_t interval,
    uint8_t reset)
{
    uint32_t now = HAL_GetTick();
    size_t length = strlen(text);
    size_t maxOffset = (length > 16U) ? length - 16U : 0U;
    char line[17];

    if (reset)
    {
        state->offset = 0;
        state->direction = 1;
        state->lastTick = now;
    }
    else
    {
        if ((uint32_t)(now - state->lastTick) < interval)
        {
            return;
        }

        state->lastTick = now;

        if (maxOffset > 0U)
        {
            if (state->offset >= maxOffset)
            {
                state->direction = -1;
            }
            else if (state->offset == 0U)
            {
                state->direction = 1;
            }

            state->offset =
                (uint16_t)((int)state->offset + state->direction);
        }
    }

    /* สร้างข้อความขนาด 16 ช่องสำหรับ LCD */
    for (uint8_t i = 0; i < 16U; i++)
    {
        size_t index = state->offset + i;
        line[i] = (index < length) ? text[index] : ' ';
    }

    line[16] = '\0';

    lcd_put_cur(row, 0);
    lcd_send_string(line);
}

#endif /* INC_I2C_LCD_H_ */
