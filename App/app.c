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


SSD1306_INIT_t ssd_init = {&hi2c1,0x7a,64,16};
SSD1306_INIT_t small_init = {&hi2c1,0x78,32,0};

DSPInitStruct_t dsp_init = {&ssd1306_driver, &ssd_init};
DSPInitStruct_t dsp_small_init = {&ssd1306_driver, &small_init};

uint8_t dsp_handle = 0;
uint8_t small_handle = 0;
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
	dspSetBrightnes(dsp_handle, 50);

	small_handle = dspOpen(&dsp_small_init);
	dspClearScreen(small_handle);
	dspPushScreen(small_handle);
	dspSetBrightnes(small_handle, 50);

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
	uint8_t ant_level = enters/25;

	dspClearScreen(dsp_handle);



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
	dspDrawString(dsp_handle, 64, 7, &font_Courier_New_10pt, (uint8_t*)"\"SSD 1306\"",DSP_TextAllignCenter);

	if (ant_level > 0) dspFillRectangle(dsp_handle, 15,10,1,3);
	if (ant_level > 1) dspFillRectangle(dsp_handle, 19,7,1,6);
	if (ant_level > 2) dspFillRectangle(dsp_handle, 23,4,1,9);
	if (ant_level > 3) dspFillRectangle(dsp_handle, 27,1,1,12);

	dspPushScreen(dsp_handle);

	//draw small
	dspClearScreen(small_handle);
	//draw battery
	dspDrawRectangle(small_handle, 104, 2, 18, 12);
	dspFillRectangle(small_handle, 102, 6, 2, 4);
	if (enters > 24) dspFillRectangle(small_handle, 118,4, 2, 8);
	if (enters > 49) dspFillRectangle(small_handle, 114,4, 2, 8);
	if (enters > 74) dspFillRectangle(small_handle, 110,4, 2, 8);
	if (enters > 99) dspFillRectangle(small_handle, 106,4, 2, 8);

	dspDrawBitmap(small_handle, 0,1, &sattelite_bmp);
	dspDrawString(dsp_handle, 60, 7, &font_Courier_New_10pt, (uint8_t*)"\"SSD 1306\"",DSP_TextAllignCenter);
	dspDrawString(small_handle, 64, 24, &font_Courier_New_10pt, (uint8_t*)"Тоже работет!!!", DSP_TextAllignCenter);

	if (ant_level > 0) dspFillRectangle(small_handle, 15,10,1,3);
	if (ant_level > 1) dspFillRectangle(small_handle, 19,7,1,6);
	if (ant_level > 2) dspFillRectangle(small_handle, 23,4,1,9);
	if (ant_level > 3) dspFillRectangle(small_handle, 27,1,1,12);

	dspPushScreen(small_handle);

	enters++;
	if (enters > 125)
	{
		enters = 0;
		angle_code = 2 - angle_code;
		dspRotate(dsp_handle, angle_code);
		dspRotate(small_handle, angle_code);
	}
}
