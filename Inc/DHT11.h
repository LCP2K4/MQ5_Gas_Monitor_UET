/*
 * DHT11.h
 *
 *  Created on: Jun 07, 2025
 *      Author: Admin
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

void DHT11_GPIO_Output(void);
void DHT11_GPIO_Input(void);
void DHT11_Start(void);
uint8_t DHT11_Check_Response(void);
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Read_Humidity(void);
uint8_t DHT11_Read_Temp(void);
#endif /* INC_DHT11_H_ */
