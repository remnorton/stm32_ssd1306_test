/*
 * app.h
 *
 *  Created on: 3 но€б. 2018 г.
 *      Author: Rem Norton
 */

#ifndef APP_H_
#define APP_H_
#include "hal_includes.h"


#define LED1	LED1_GPIO_Port, LED1_Pin
#define ST_RST	ST_RST_GPIO_Port, ST_RST_Pin
#define BK		BK_GPIO_Port, BK_Pin
#define DC		DC_GPIO_Port, DC_Pin


extern I2C_HandleTypeDef hi2c1;
//extern SPI_HandleTypeDef hspi1;

void app_init();
void app_step();

#endif /* APP_H_ */
