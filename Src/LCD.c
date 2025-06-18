/*
 * LCD.c
 *
 *  Created on: Jun 07, 2025
 *      Author: Admin
 */

#include"stm32f4xx.h"
#include"Delay.h"
#include"I2C.h"
void LCD_Write(uint8_t addr, uint8_t *data, int size) {
    i2c_start();
    i2c_adress(addr);
    for (int i = 0; i < size; i++) i2c_write(data[i]);
    i2c_stop();
}


void lcd_send_cmd(char cmd) {
    uint8_t data_t[4];
    data_t[0] = (cmd & 0xF0) | 0x0C;
    data_t[1] = (cmd & 0xF0) | 0x08;
    data_t[2] = ((cmd << 4) & 0xF0) | 0x0C;
    data_t[3] = ((cmd << 4) & 0xF0) | 0x08;
    LCD_Write(0x4E, data_t, 4);
}

void lcd_send_data(char data) {
    uint8_t data_t[4];
    data_t[0] = (data & 0xF0) | 0x0D;
    data_t[1] = (data & 0xF0) | 0x09;
    data_t[2] = ((data << 4) & 0xF0) | 0x0D;
    data_t[3] = ((data << 4) & 0xF0) | 0x09;
    LCD_Write(0x4E, data_t, 4);
}

void lcd_put_cur(int row, int col) {
    lcd_send_cmd((row == 0) ? (0x80 + col) : (0xC0 + col));
}

void lcd_send_string(char *str) {
    while (*str) lcd_send_data(*str++);
}

void lcd_init(void) {
    Delay_ms(50);
    lcd_send_cmd(0x30); Delay_ms(5);
    lcd_send_cmd(0x30); Delay_us(200);
    lcd_send_cmd(0x30); Delay_ms(10);
    lcd_send_cmd(0x20); Delay_ms(10);

    lcd_send_cmd(0x28);
    lcd_send_cmd(0x08);
    lcd_send_cmd(0x01); Delay_ms(2);
    lcd_send_cmd(0x06);
    lcd_send_cmd(0x0C);
}
