/*
 * I2C.h
 *
 *  Created on: Jun 07, 2025
 *      Author: Admin
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_
void i2c_config(void);

void i2c_start(void);
void i2c_adress(uint8_t addr);
void i2c_write(uint8_t data);
void i2c_stop(void);

#endif /* INC_I2C_H_ */
