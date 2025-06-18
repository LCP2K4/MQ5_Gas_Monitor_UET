/*
 * I2C.c
 *
 *  Created on: Jun 07, 2025
 *      Author: Admin
 */
#include "stm32f4xx.h"
#include "delay.h"

void i2c_config(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
	GPIOB->MODER |= (2 << (8 * 2)) | (2 << (9 * 2));
	GPIOB->OTYPER |= (1 << 8) | (1 << 9);
	GPIOB->OSPEEDR |= (3 << (8 * 2)) | (3 << (9 * 2));
	GPIOB->PUPDR |= (1 << (8 * 2)) | (1 << (9 * 2));
	GPIOB->AFR[1] |= (4 << ((8 - 8) * 4)) | (4 << ((9 - 8) * 4));
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    I2C1->CR1 |= I2C_CR1_SWRST;
    Delay_ms(1);
    I2C1->CR1 &= ~I2C_CR1_SWRST;
    I2C1->CR1 &= ~(1 << 15);
    I2C1->CR2 = 50;
    I2C1->CCR = 250;
    I2C1->TRISE = 51;
    I2C1->CR1 |= I2C_CR1_PE;
}

void i2c_start(void) {
    I2C1->CR1 |= (1 << 10) | (1 << 8);
    while (!(I2C1->SR1 & (1 << 0)));
}

void i2c_adress(uint8_t addr) {
    I2C1->DR = addr;
    while (!(I2C1->SR1 & (1 << 1)));
    (void)I2C1->SR2;
}

void i2c_write(uint8_t data) {
    while (!(I2C1->SR1 & (1 << 7)));
    I2C1->DR = data;
    while (!(I2C1->SR1 & (1 << 2)));
}

void i2c_stop(void) {
    I2C1->CR1 |= (1 << 9);
}

