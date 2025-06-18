/*
 * Delay.c
 *
 *  Created on: Jun 17, 2025
 *      Author: Admin
 */
#include "stm32f4xx.h"

void TIM2_config(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 16000 - 1; // 16MHz / 16000 = 1kHz
    TIM2->ARR = 200 - 1;   // 200ms
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->CR1 |= TIM_CR1_CEN;
}
void Delay_us(uint32_t us) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    TIM4->PSC = 16 - 1;
    TIM4->ARR = us;
    TIM4->EGR = TIM_EGR_UG;
    TIM4->SR = 0;
    TIM4->CR1 = TIM_CR1_CEN;
    while (!(TIM4->SR & TIM_SR_UIF));
    TIM4->CR1 = 0;
}

void Delay_ms(uint32_t ms) {
    Delay_us(ms * 1000);
}


