/*
 * UART.c
 *
 *  Created on: Jun 07, 2025
 *      Author: Admin
 */
#include"stm32f4xx.h"
void USART2_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER |= (2 << (2 * 2));
    GPIOA->AFR[0] |= (7 << (4 * 2));

    USART2->BRR = (104 << 4) | 3;
    USART2->CR1 |= USART_CR1_TE | USART_CR1_UE;
}

void USART2_SendChar(char c) {
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = c;
}

void USART2_SendString(const char *str) {
    while (*str) USART2_SendChar(*str++);
}

