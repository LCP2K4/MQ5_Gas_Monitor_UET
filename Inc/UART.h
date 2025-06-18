/*
 * UART.h
 *
 *  Created on: Jun 07, 2025
 *      Author: Admin
 */

#ifndef INC_UART_H_
#define INC_UART_H_

void USART2_Init(void);

void USART2_SendChar(char c);

void USART2_SendString(const char *str);

#endif /* INC_UART_H_ */
