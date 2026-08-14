#include "lcd_i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void lcd_write_nibble(i2c_port_t i2c_num, uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (LCD_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, data | LCD_BACKLIGHT, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

static void lcd_send_cmd(i2c_port_t i2c_num, uint8_t data) {
    uint8_t high_nibble = data & 0xF0;
    uint8_t low_nibble = (data << 4) & 0xF0;
    lcd_write_nibble(i2c_num, high_nibble | En);
    lcd_write_nibble(i2c_num, high_nibble);
    lcd_write_nibble(i2c_num, low_nibble | En);
    lcd_write_nibble(i2c_num, low_nibble);
}

static void lcd_send_data(i2c_port_t i2c_num, uint8_t data) {
    uint8_t high_nibble = data & 0xF0;
    uint8_t low_nibble = (data << 4) & 0xF0;
    lcd_write_nibble(i2c_num, high_nibble | En | Rs);
    lcd_write_nibble(i2c_num, high_nibble | Rs);
    lcd_write_nibble(i2c_num, low_nibble | En | Rs);
    lcd_write_nibble(i2c_num, low_nibble | Rs);
}

void lcd_init(i2c_port_t i2c_num) {
    vTaskDelay(50 / portTICK_PERIOD_MS);
    lcd_write_nibble(i2c_num, 0x30 | En);
    lcd_write_nibble(i2c_num, 0x30);
    vTaskDelay(5 / portTICK_PERIOD_MS);
    lcd_write_nibble(i2c_num, 0x30 | En);
    lcd_write_nibble(i2c_num, 0x30);
    vTaskDelay(1 / portTICK_PERIOD_MS);
    lcd_write_nibble(i2c_num, 0x30 | En);
    lcd_write_nibble(i2c_num, 0x30);
    lcd_write_nibble(i2c_num, 0x20 | En);
    lcd_write_nibble(i2c_num, 0x20);

    lcd_send_cmd(i2c_num, LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
    lcd_send_cmd(i2c_num, LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
    lcd_send_cmd(i2c_num, LCD_CLEARDISPLAY);
    vTaskDelay(2 / portTICK_PERIOD_MS);
    lcd_send_cmd(i2c_num, LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);
}

void lcd_clear(i2c_port_t i2c_num) {
    lcd_send_cmd(i2c_num, LCD_CLEARDISPLAY);
    vTaskDelay(2 / portTICK_PERIOD_MS);
}

void lcd_set_cursor(i2c_port_t i2c_num, uint8_t col, uint8_t row) {
    int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    if (row > 3) {
        row = 3;
    }
    lcd_send_cmd(i2c_num, LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void lcd_print(i2c_port_t i2c_num, const char *str) {
    while (*str) {
        lcd_send_data(i2c_num, (uint8_t)(*str));
        str++;
    }
}
