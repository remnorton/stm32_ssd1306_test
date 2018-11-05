/*
 * ssd1306.h
 *
 *  Created on: 07.09.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */


#ifndef SSD1306_H
#define SSD1306_H
#include "dsp_driver.h"

typedef struct
{
    I2C_HandleTypeDef* i2c;
    uint8_t dsp_addr;
}SSD1306_INIT_t;

#endif // SSD1306_H
