/*
 * app.c
 *
 *  Created on: 3 нояб. 2018 г.
 *      Author: Rem Norton
 */
#include "app.h"
#include "bitmaps.h"
#include "ssd1306.h"
#include "fonts_lib.h"
#include "lcdlib.h"

static uint32_t led_time = 0;
static uint32_t draw_time = 0;
static uint8_t enters = 0;


SSD1306_INIT_t ssd_init = {&hi2c1,0x78,64,16};
DSPInitStruct_t dsp_init = {&ssd1306_driver, &ssd_init};

uint8_t dsp_handle = 0;
uint8_t st_handle = 0;

uint8_t angle_code = 0;

//
//Private forwards
//

void draw_scene();

//
//Public members
//
void app_init()
{
	//init ssd1306
	dspInit();
	dsp_handle = dspOpen(&dsp_init);
	dspClearScreen(dsp_handle);
	dspPushScreen(dsp_handle);

	dspSetBrightnes(dsp_handle, 0);
}

void app_step()
{
	if (HAL_GetTick() > led_time)
	{
		HAL_GPIO_TogglePin(LED1);
		led_time = HAL_GetTick() + 100;
	}

	if (HAL_GetTick() > draw_time)
	{
		draw_scene();
		draw_time = HAL_GetTick()+100;
	}
}

//
//Private members
//

void draw_scene()
{
	dspClearScreen(dsp_handle);

	//dspDrawPixel(dsp_handle, 0,0, DSP_COLOR_White);
	//dspDrawPixel(dsp_handle, 2,2, DSP_COLOR_White);

	//draw battery
	dspDrawRectangle(dsp_handle, 104, 2, 18, 12);
	dspFillRectangle(dsp_handle, 102, 6, 2, 4);
	if (enters > 24) dspFillRectangle(dsp_handle, 118,4, 2, 8);
	if (enters > 49) dspFillRectangle(dsp_handle, 114,4, 2, 8);
	if (enters > 74) dspFillRectangle(dsp_handle, 110,4, 2, 8);
	if (enters > 99) dspFillRectangle(dsp_handle, 106,4, 2, 8);
	dspDrawString(dsp_handle, 64, 30, &font_Courier_New_10pt, (uint8_t*)"УРА !!!",DSP_TextAllignCenter);
	dspDrawString(dsp_handle, 64, 48, &font_Courier_New_10pt, (uint8_t*)"Оно работает!", DSP_TextAllignCenter);
	dspDrawRectangle(dsp_handle, 0, 16, 127, 47);
	dspDrawRectangle(dsp_handle, 2, 18, 123, 43);
	dspDrawBitmap(dsp_handle, 0,1, &sattelite_bmp);
	dspDrawString(dsp_handle, 60, 7, &font_Courier_New_10pt, (uint8_t*)"\"SSD 1306\"",DSP_TextAllignCenter);

	dspPushScreen(dsp_handle);

	enters++;
	if (enters > 125)
	{
		enters = 0;
		angle_code = 2 - angle_code;
		dspRotate(dsp_handle, angle_code);
	}
}
