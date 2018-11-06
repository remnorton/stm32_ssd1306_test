/*
 * display.h
 *
 *  Created on: 08.09.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */


#ifndef DISPLAY_H
#define DISPLAY_H
#include "dsp_driver.h"


void dspInit();
uint8_t dspOpen(DSPInitStruct_t* init);
void dspClose(uint8_t handle);
uint16_t dspGetId(uint8_t handle);
uint16_t dspGetScreenWidth(uint8_t handle);
uint16_t dspGetScreenHeight(uint8_t handle);

void dspSetColor(uint8_t handle, uint32_t color);
void dspSetBkColor(uint8_t handle, uint32_t color);

void dspClearScreen(uint8_t handle);

void dspSetInverse(uint8_t handle, uint8_t inverse);
void dspSwitchOn(uint8_t handle, uint8_t active);
void dspSetBrightnes(uint8_t handle, uint32_t level);

void dspDrawPixel(uint8_t handle, uint32_t x, uint32_t y, uint32_t color);
void dspDrawLine(uint8_t handle, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

void dspDrawRectangle(uint8_t handle, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void dspFillRectangle(uint8_t handle, uint32_t x, uint32_t y, uint32_t width, uint32_t height);


uint16_t dspDrawChar(uint8_t handle, uint32_t x, uint32_t y, Font_type_t* font, uint8_t sym);
void dspDrawString(uint8_t handle, uint32_t x, uint32_t y, Font_type_t* font, uint8_t* text, uint8_t allign);

void dspSendCommands(uint8_t handle, uint8_t* commands, uint8_t len);
void dspSendData(uint8_t handle, uint8_t* data, uint16_t len);
void dspPushScreen(uint8_t handle);

uint32_t dspRgbaColor(uint8_t handle, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
uint32_t dspRgbColor(uint8_t handle, uint8_t red, uint8_t green, uint8_t blue);
uint32_t dspPixColor(uint8_t handle, uint32_t x, uint32_t y);

uint32_t dspBitPerPixel(uint8_t handle);
void dspDrawBitmap(uint8_t handle, uint32_t x, uint32_t y, DSP_Bitmap_t* bmp);
void dspDrawImage(uint8_t handle, uint32_t x, uint32_t y, DSP_Image_t* img);

void dspRotate(uint8_t handle, uint8_t angle_code);

//
//
//
void dspDrawElipse(uint8_t handle, uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad);
void dspDrawCyrcle(uint8_t handle, uint32_t x, uint32_t y, uint32_t rad);

void dspFillElipse(uint8_t handle, uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad);
void dspFillCyrcle(uint8_t handle, uint32_t x, uint32_t y, uint32_t rad);

void dspDrawArc(uint8_t handle, uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad, uint16_t start, uint32_t stop);
void dspFillArc(uint8_t handle, uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad, uint16_t start, uint32_t stop);

void dspFillRectangleGradient(uint8_t handle, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint8_t orientation, uint32_t startColor, uint32_t stopColor);

#endif // DISPLAY_H
