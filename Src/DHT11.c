/*
 * DHT11.c
 *
 *  Created on: Jun 07, 2025
 *      Author: Admin
 */
#include"stm32f4xx.h"
#include"Delay.h"
void DHT11_GPIO_Output(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER &= ~(3 << (1 * 2));
    GPIOA->MODER |=  (1 << (1 * 2));
    GPIOA->OTYPER &= ~(1 << 1);
    GPIOA->OSPEEDR |= (1 << (1 * 2));
    GPIOA->PUPDR &= ~(3 << (1 * 2));
}

void DHT11_GPIO_Input(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->MODER &= ~(3 << (1 * 2));
    GPIOA->PUPDR &= ~(3 << (1 * 2));
    GPIOA->PUPDR |=  (1 << (1 * 2));
}

void DHT11_Start(void)
{
    Delay_us(200);
    DHT11_GPIO_Output();
    GPIOA->BSRR = (1 << (1 + 16)); // Reset PA1 (pull low)
    Delay_ms(18);
    GPIOA->BSRR = (1 << 1);        // Set PA1 (pull high)
    Delay_us(30);
    DHT11_GPIO_Input();           // Set as input to read
}

uint8_t DHT11_Check_Response(void) {
    uint8_t response = 0;
    uint32_t timeout = 10000; // Timeout counter (~100ms at 1µs per loop)


    while (GPIOA->IDR & (1 << 1)) {
        if (--timeout == 0) return 0; // Timeout if no response
        Delay_us(1);
    }

    // Wait for high signal
    timeout = 10000;
    while (!(GPIOA->IDR & (1 << 1))) {
        if (--timeout == 0) return 0;
        Delay_us(1);
    }

    // Wait for low signal
    timeout = 10000;
    while (GPIOA->IDR & (1 << 1)) {
        if (--timeout == 0) return 0;
        Delay_us(1);
    }

    response = 1;
    return response;
}

uint8_t DHT11_Read_Byte(void) {
    uint8_t result = 0;
    uint32_t timeout = 10000;

    for (uint8_t i = 0; i < 8; i++) {
        // Wait for high signal
        while (!(GPIOA->IDR & (1 << 1))) {
            if (--timeout == 0) return 0;
            Delay_us(1);
        }

        Delay_us(30);
        if (GPIOA->IDR & (1 << 1)) {
            result |= (1 << (7 - i));
        }

        // Wait for low signal
        timeout = 10000;
        while (GPIOA->IDR & (1 << 1)) {
            if (--timeout == 0) return 0;
            Delay_us(1);
        }
    }

    return result;
}
uint8_t DHT11_Read_Humidity(void)
{
    uint8_t rh_int, rh_dec, temp_int, temp_dec, checksum;

    DHT11_Start();
    if (DHT11_Check_Response())
    {
        rh_int    = DHT11_Read_Byte();
        rh_dec    = DHT11_Read_Byte();
        temp_int  = DHT11_Read_Byte();
        temp_dec  = DHT11_Read_Byte();
        checksum  = DHT11_Read_Byte();

        if ((rh_int + rh_dec + temp_int + temp_dec) == checksum)
            return rh_int;
    }

    return 0xFF;
}

uint8_t DHT11_Read_Temp(void)
{
    uint8_t rh_int, rh_dec, temp_int, temp_dec, checksum;

    DHT11_Start();
    if (DHT11_Check_Response())
    {
        rh_int    = DHT11_Read_Byte();
        rh_dec    = DHT11_Read_Byte();
        temp_int  = DHT11_Read_Byte();
        temp_dec  = DHT11_Read_Byte();
        checksum  = DHT11_Read_Byte();

        if ((rh_int + rh_dec + temp_int + temp_dec) == checksum)
            return temp_int;
    }

    return 0xFF;
}


