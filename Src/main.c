#include "stm32f4xx.h"
#include <stdio.h>
#include <math.h>
#include "Delay.h"
#include "I2C.h"
#include "LCD.h"
#include "DHT11.h"
#include "UART.h"
volatile uint8_t paused = 0;
volatile uint8_t reset = 0;
volatile uint8_t update = 0;

void config_GPIO(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

//Button 4,7
    GPIOA->MODER &= ~(3 << (7 * 2) | 3 << (4 * 2));
    GPIOA->PUPDR |=  ((1 << (4 * 2)) | (1 << (7 * 2)));
// Buzzer
    GPIOA->MODER |= (1 << (6 * 2));
//relay
    GPIOB->MODER &= ~(3 << (5* 2));
    GPIOB->MODER |= (1 << (5 * 2));
//LED
    GPIOB->MODER |= (1 << (0 * 2)) | (1 << (1 * 2)) | (1 << (2 * 2)) | (1 << (10 * 2));
//ADC
    GPIOA->MODER |= (3 << 0);
    ADC1->SQR3 = 0;
    ADC1->SMPR2 |= (7 << 0);
    ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t Read_ADC(void) {
    ADC1->CR2 |= (1 << 30);
    while (!(ADC1->SR & (1 << 1)));
    return ADC1->DR;
}
//Cau hinh interrupt
void init_interrupt()
{
  RCC->APB2ENR |= 1 << 14;
  SYSCFG->EXTICR[0] &= ~0xFF;
  SYSCFG->EXTICR[1] &= ~((0xF << 0) | (0xF << 12));
  EXTI->IMR |= (1 << 4) | (1 << 7);      // Unmask EXTI6, EXTI7
  EXTI->RTSR |= (1 << 4) | (1 << 7);
}
//xu ly interrupt

void EXTI4_IRQHandler(void)
{
	if (EXTI->PR & (1 << 4)) {
	        EXTI->PR |= (1 << 4);   // Clear pending
	        reset = 1;
	    }
}
void EXTI9_5_IRQHandler(void)
{

    if (EXTI->PR & (1 << 7)) {
        EXTI->PR |= (1 << 7);   // Clear pending
        paused ^= 1;
    }
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        update = 1;
    }
}

int main(void) {
    config_GPIO();
    TIM2_config();
    USART2_Init();
    i2c_config();
    lcd_init();
    init_interrupt();
    DHT11_GPIO_Output();
    DHT11_GPIO_Input();
    DHT11_Start();
    NVIC_SetPriority(EXTI4_IRQn,0);
    NVIC_EnableIRQ(EXTI4_IRQn);
    NVIC_SetPriority(EXTI9_5_IRQn, 1);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 2);
    char buffer[32];
    int status = -1;

    while (1) {
      if (update) {
    	update = 0;
    	uint8_t humidity = DHT11_Read_Humidity();
    	uint8_t temp = DHT11_Read_Temp();
        uint16_t adc_value = Read_ADC();
        float Vout = (adc_value / 4095.0f) * 3.3f;
        float R_G = (3.3f - Vout)/Vout;
        float ratio = R_G/3.75;
        float ratio_last = ratio;
        if (((float)temp) > 20) {
            float temp_last = 1.0 + 0.05 * (((float)temp - 20.0) / 10.0);
            ratio_last *= (temp_last < 0.8 ? 0.8 : temp_last);
            if (humidity > 50 && humidity < 75) {
                ratio_last *= 1.0;
            } else if (humidity < 50) {
                ratio_last *= 0.95;
            } else {
                ratio_last *= 1.05;
            }
        } else {
            ratio_last *= 1.0;
            if (humidity > 50 && humidity < 75) {
                ratio_last *= 1.0;
            } else if (humidity < 50) {
                ratio_last *= 0.95;
            } else {
                ratio_last *= 1.05;
            }
        }
        float value = pow(10, -2.0 * log10(ratio_last) + 1.7);
        if (reset) {
                reset = 0;
                __disable_irq();
                NVIC_SystemReset();
                Delay_ms(50);
            }

        if (paused) {
        	        GPIOB->BSRR = (1 << 0);
        	        GPIOB->BSRR = (1 << (1 + 16)) | (1 << (2 + 16)) | (1 << (10 + 16));
        	        GPIOA->BSRR = (1 << (6 + 16));
                    lcd_put_cur(0, 0);
                    lcd_send_string("Trang thai:   ");
                    lcd_put_cur(1, 0);
                    lcd_send_string("Tam dung      ");
                    Delay_ms(500);
                    while (paused) {
                    	 if (reset) break;
                           __WFI();
                       }
                    continue;
                }

        if (value < 300) {
                status = 0;
                GPIOB->BSRR = (1 << 1);
                GPIOA->BSRR = (1 << (6 + 16));
                GPIOB->BSRR = (1 << (0 + 16)) | (1 << (10 + 16)) | (1 << (2 + 16));
                GPIOB->BSRR = (1 << (5 + 16));
            } else if (value < 500) {
                status = 1;
                GPIOB->BSRR = (1 << 2);
                GPIOA->BSRR = (1 << 6);
                GPIOB->BSRR = (1 << (0 + 16)) | (1 << (10 + 16)) | (1 << (1 + 16));
                GPIOB->BSRR = (1 << (5+16));
            } else if (value < 1000) {
                status = 2;
                GPIOB->ODR ^= (1 << 10);
                GPIOA->BSRR = (1 << 6);
                GPIOB->BSRR = (1 << (0 + 16)) | (1 << (1 + 16)) | (1 << (2 + 16));
                GPIOB->BSRR = (1 << (5));
                Delay_ms(1000);
            } else {
                status = 3;
                GPIOB->ODR ^= (1 << 10);
                GPIOA->BSRR = (1 << 6);
                GPIOB->BSRR = (1 << (0 + 16)) | (1 << (1 + 16)) | (1 << (2 + 16));
                GPIOB->BSRR = (1 << (5));
                Delay_ms((uint32_t)(1000 * (1.0f - (float)adc_value / 4200)));
        }
        // Send UART
        if(value <= 10000) {
             sprintf(buffer, "PPM: %.2f | %RH: %d | Temp: %d\r\n", value, humidity, temp);
        }
        else {
        	sprintf(buffer, "PPM: %.2f | %RH: %d | Temp: %d\r\n", 10000.00, humidity, temp);
        }
        USART2_SendString(buffer);
        // LCD Display
        lcd_put_cur(0, 0);
        lcd_send_string("Trang thai:     ");
        lcd_put_cur(1, 0);
        switch (status) {
            case 0: lcd_send_string("Binh thuong "); break;
            case 1: lcd_send_string("Ro ri ga nhe"); break;
            case 2: lcd_send_string("Ro ri nhieu "); break;
            case 3: lcd_send_string("BAO DONG !!! "); break;
            default: lcd_send_string("Tam dung     "); break;
        }
    } else {
    	__WFI();
    }
  }
}
