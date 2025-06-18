/*
 * LCD.h
 *
 *  Created on: Jun 07, 2025
 *      Author: Admin
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_
void LCD_Write(uint8_t addr, uint8_t *data, int size);

void lcd_send_cmd(char cmd);

void lcd_send_data(char data);

void lcd_put_cur(int row, int col);

void lcd_send_string(char *str);

void lcd_init(void);


#endif /* INC_LCD_H_ */
