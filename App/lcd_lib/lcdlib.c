/*
 * lcdlib.cpp
 *
 *  Created on: 08.09.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */
#include <string.h>
#include "lcdlib.h"
#include "intmath.h"

//#include <QDebug>

//
//
//

typedef struct
{
    DSPInitStruct_t* init;
    uint32_t        bkColor;
    uint32_t        foreColor;
}DSP_InfoStruct_t;

#define _MAX_DSP_   5
static DSP_InfoStruct_t dsp[_MAX_DSP_];
static uint8_t dd_idx = 0;

//
//
//

void dspInit()
{
    for (int i = 0; i < _MAX_DSP_; i++)
    {
        dsp[i].bkColor = DSP_COLOR_Black;
        dsp[i].foreColor = DSP_COLOR_White;
        dsp[i].init = 0;
    }
}

uint8_t dspOpen(DSPInitStruct_t* init)
{
    if (!init) return 0;
    if (!init->driver) return 0;
    if (!init->driver->init) return 0;
    if (dd_idx > _MAX_DSP_) return 0;
    init->driver->init(init->drvSettings);
    dsp[dd_idx++].init = init;
    return dd_idx;
}

void dspClose(uint8_t handle)
{
	if (!handle) return;
	if (!dsp[handle-1].init) return;
	if (dsp[handle-1].init->driver->deInit) dsp[handle-1].init->driver->deInit(dsp[handle-1].init->drvSettings);
	dsp[handle-1] = (DSP_InfoStruct_t){0,0,0};
}

uint16_t dspGetId(uint8_t handle)
{
    if (!handle) return 0;
    if (!dsp[handle-1].init) return 0;
    return dsp[handle-1].init->driver->getId(dsp[handle-1].init->drvSettings);
}

uint16_t dspGetScreenWidth(uint8_t handle)
{
    if (!handle) return 0;
    if (!dsp[handle-1].init) return 0;
    return dsp[handle-1].init->driver->screenWidth(dsp[handle-1].init->drvSettings);
}

uint16_t dspGetScreenHeight(uint8_t handle)
{
    if (!handle) return 0;
    if (!dsp[handle-1].init) return 0;
    return dsp[handle-1].init->driver->screenHeight(dsp[handle-1].init->drvSettings);
}

void dspSetColor(uint8_t handle, uint32_t color)
{
    if (!handle) return;
    if (!dsp[handle-1].init) return;
    dsp[handle-1].foreColor = color;
}

void dspSetBkColor(uint8_t handle, uint32_t color)
{
    if (!handle) return;
    if (!dsp[handle-1].init) return;
    dsp[handle-1].bkColor = color;
}

void dspClearScreen(uint8_t handle)
{
    if (!handle) return;
    if (!dsp[handle-1].init) return;
    dsp[handle-1].init->driver->clearScreen(dsp[handle-1].init->drvSettings,dsp[handle-1].bkColor);
}

void dspSetInverse(uint8_t handle, uint8_t inverse)
{
    if (!handle) return;
    if (!dsp[handle-1].init) return;
    if (dsp[handle-1].init->driver->setInverse) dsp[handle-1].init->driver->setInverse(dsp[handle-1].init->drvSettings,inverse);
}

void dspSwitchOn(uint8_t handle, uint8_t active)
{
    if (!handle) return;
    if (!dsp[handle-1].init) return;
    if (dsp[handle-1].init->driver->switchOn) dsp[handle-1].init->driver->switchOn(dsp[handle-1].init->drvSettings,active);
}

void dspSetBrightnes(uint8_t handle, uint32_t level)
{
    if (!handle) return;
    if (!dsp[handle-1].init) return;
    if (dsp[handle-1].init->driver->setBrightnes) dsp[handle-1].init->driver->setBrightnes(dsp[handle-1].init->drvSettings,level);
}

void dspDrawPixel(uint8_t handle, uint32_t x, uint32_t y, uint32_t color)
{
    if (!handle) return;
    if (!dsp[handle-1].init) return;
    if (dsp[handle-1].init->driver->drawPixel) dsp[handle-1].init->driver->drawPixel(dsp[handle-1].init->drvSettings,x,y,color);
}

void dspDrawLine(uint8_t handle, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    if (!handle) return;
    if (!dsp[handle-1].init) return;
    if (dsp[handle-1].init->driver->drawLine) dsp[handle-1].init->driver->drawLine(dsp[handle-1].init->drvSettings,x1,y1,x2,y2,dsp[handle-1].foreColor);
    else
    {
        int32_t dx = (x2-x1);
        int32_t dy = (y2-y1);

        int8_t  sx = (dx == 0)?0:((dx>0)?1:-1);
        int8_t  sy = (dy == 0)?0:((dy>0)?1:-1);

        int32_t x = x1;
        int32_t y = y1;


        if (dx && dy)
        {
            if (dx == dy)
            {
                while ((x != x2) || y != y2)
                {
                   dspDrawPixel(handle, x,y, dsp[handle-1].foreColor);
                   x += sx;
                   y += sy;
                }
            }
            else
            {
                if (ABS(dy) < ABS(dx)) //f(x) = ((x-x1)*dy)/dx+y1;
                {
                    int32_t prev_y = y1;
                    while (x != x2)
                    {
                        y = ((x*10-(int32_t)(x1*10))*dy*10)/(dx*10)+(int32_t)y1*10;
                        uint8_t tail = ABS(y)%10;
                        y = y/10 + 1*((tail>5)?1:0);
                        if (ABS(prev_y - y) > 1)
                            dspDrawLine(handle, x,prev_y,x,y);
                        else
                            dspDrawPixel(handle,x,y, dsp[handle-1].foreColor);
                        prev_y = y;
                        x += sx;
                    }
                }
                else //f(y) = ((y-y1)*xy)/dy+x1;
                {
                    int32_t prev_x = x1;
                    while (y != y2)
                    {
                        x = (( y*10-(int32_t)(y1*10))*dx*10)/(dy*10)+(int32_t)x1*10;
                        uint8_t tail = ABS(x)%10;
                        x = x/10 + 1*((tail>5)?1:0);
                        if (ABS(prev_x - x) > 1)
                            dspDrawLine(handle, prev_x,y,x,y);
                        else
                            dspDrawPixel(handle,x,y, dsp[handle-1].foreColor);
                        prev_x = x;
                        y += sy;
                    }
                }
            }
        }
        else
        {
            dspDrawPixel(handle,x,y, dsp[handle-1].foreColor);
            if (dx == 0) //vertical
            {
                while (y != y2)
                {
                    y += sy;
                    dspDrawPixel(handle,x,y,dsp[handle-1].foreColor);
                }
            }
            else //horizontal
            {
                while (x != x2)
                {
                    x += sx;
                    dspDrawPixel(handle,x,y,dsp[handle-1].foreColor);
                }
            }
        }

    }
}

void dspDrawRectangle(uint8_t handle, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    if (!handle) return;
    if (!dsp[handle-1].init) return;
    if (dsp[handle-1].init->driver->drawRectangle) dsp[handle-1].init->driver->drawRectangle(dsp[handle-1].init->drvSettings,x,y,width,height,dsp[handle-1].foreColor);
    else
    {
        dspDrawLine(handle,x,y,x+width,y);
        dspDrawLine(handle,x+width,y,x+width,y+height);
        dspDrawLine(handle,x+width,y+height,x,y+height);
        dspDrawLine(handle,x,y+height,x,y);
    }
}

void dspFillRectangle(uint8_t handle, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    if (!handle) return;
    if (!dsp[handle-1].init) return;
    if (dsp[handle-1].init->driver->fillRectangle) dsp[handle-1].init->driver->fillRectangle(dsp[handle-1].init->drvSettings,x,y,width,height,dsp[handle-1].foreColor);
    else
        for (uint16_t ly = y; ly <=(y+ height); ly++)
            dspDrawLine(handle,x,ly,x+width,ly);
}

uint16_t dspDrawChar(uint8_t handle, uint32_t x, uint32_t y, Font_type_t* font, uint8_t sym)
{
    if (!handle) return 0;
    if (!dsp[handle-1].init) return 0;
    if (dsp[handle-1].init->driver->drawChar) return dsp[handle-1].init->driver->drawChar(dsp[handle-1].init->drvSettings,x,y,font,sym,dsp[handle-1].foreColor);
    else
    {
        if (!font) return 0;
        if (sym > font->stop) return 0;
        uint8_t vs = (font->descr)?1:0;
        uint8_t sym_width = vs?font->descr[(sym-font->start)].width:font->width;
        uint8_t sym_height = vs?font->descr[(sym-font->start)].height:font->height;
        uint16_t width = sym_width;

        while (width % 8) width++;
        uint8_t byte_width = width/8;

        uint16_t offset = vs?font->descr[(sym-font->start)].offset:((sym-font->start)*byte_width*font->height);


        if (((x+sym_width) > dspGetScreenWidth(handle)) || ((y+font->height) > dspGetScreenHeight(handle))) return 0;
        uint16_t startY = vs?(font->height-font->descr[(sym-font->start)].trimed - sym_height):0;

        for (int sy = 0; sy < sym_height; sy++)
        {
            uint8_t w = 0;
            for (int i = 0; i < byte_width; i++)
            {
                uint8_t b = font->bitmaps[offset + sy*byte_width + i];
                for (int sx = 0; (sx < 8) && (w < sym_width); sx++)
                {
                    if ((b << sx) & 0x80) dspDrawPixel(handle,x+sx+8*i,y+sy+startY,dsp[handle-1].foreColor);
                    w++;
                }
            }
        }
        return sym_width+font->spacing;
    }
}

void dspDrawString(uint8_t handle, uint32_t x, uint32_t y, Font_type_t* font, uint8_t* text, uint8_t allign)
{
    if (!handle) return;
    if (!dsp[handle-1].init) return;
    if (dsp[handle-1].init->driver->drawString) dsp[handle-1].init->driver->drawString(dsp[handle-1].init->drvSettings,x,y,font,text,dsp[handle-1].foreColor,allign);
    else
    {
        uint16_t ch_len = strlen((const char*)text);
        uint32_t pix_len = calc_pix_len(font, text, ch_len);
        uint32_t sx, sy;

        while (pix_len > dspGetScreenWidth(handle))
        {
            ch_len--;
            pix_len = calc_pix_len(font, text, ch_len);
        }

        sy = y;
        switch(allign)
        {
            case DSP_TextAllignLeft:
            {
                sx = x;
                break;
            }
            case DSP_TextAllignRight:
            {
                if (pix_len > x)
                    sx = 0;
                else
                    sx = x - pix_len;
                break;
            }
            case DSP_TextAllignCenter:
            {
                sx = x - (pix_len/2);
                sy -= (font->height/2);
                break;
            }
        }

        uint32_t pos = 0;
        for (int i = 0; i < ch_len; i++)
        {
            pos += dspDrawChar(handle, sx+pos, sy, font, text[i]);
        }
    }
}

void dspSendCommands(uint8_t handle, uint8_t* commands, uint8_t len)
{
    if (!handle) return;
    if (!dsp[handle-1].init) return;
    if (dsp[handle-1].init->driver->sendCommands) dsp[handle-1].init->driver->sendCommands(dsp[handle-1].init->drvSettings,commands, len);
}

void dspSendData(uint8_t handle, uint8_t* data, uint16_t len)
{
	if (!handle) return;
	if (!dsp[handle-1].init) return;
	if (dsp[handle-1].init->driver->sendData) dsp[handle-1].init->driver->sendData(dsp[handle-1].init->drvSettings,data, len);
}

void dspPushScreen(uint8_t handle)
{
    if (!handle) return;
    if (!dsp[handle-1].init) return;
    if (dsp[handle-1].init->driver->pushData) dsp[handle-1].init->driver->pushData(dsp[handle-1].init->drvSettings);
}

uint32_t dspRgbaColor(uint8_t handle, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    if (!handle) return 0;
    if (!dsp[handle-1].init) return 0;
    if (dsp[handle-1].init->driver->rgbColor) return dsp[handle-1].init->driver->rgbColor(dsp[handle-1].init->drvSettings,red, green, blue, alpha);
    return 0;
}

uint32_t dspRgbColor(uint8_t handle, uint8_t red, uint8_t green, uint8_t blue)
{
	return dspRgbaColor(handle, red, green, blue, 0xff);
}

uint32_t dspPixColor(uint8_t handle, uint32_t x, uint32_t y)
{
    if (!handle) return 0;
    if (!dsp[handle-1].init) return 0;
    if (dsp[handle-1].init->driver->pixelColor) return dsp[handle-1].init->driver->pixelColor(dsp[handle-1].init->drvSettings,x,y);
    return 0;
}

uint32_t dspBitPerPixel(uint8_t handle)
{
    if (!handle) return 0;
    if (!dsp[handle-1].init) return 0;
    if (dsp[handle-1].init->driver->bitPerPixel) return dsp[handle-1].init->driver->bitPerPixel(dsp[handle-1].init->drvSettings);
    return 0;
}

void dspDrawBitmap(uint8_t handle, uint32_t x, uint32_t y, DSP_Bitmap_t* bmp)
{
    if (!handle) return;
    if (!dsp[handle-1].init) return;
    if (dsp[handle-1].init->driver->drawBitmap) dsp[handle-1].init->driver->drawBitmap(dsp[handle-1].init->drvSettings,x,y,bmp,dsp[handle-1].foreColor);
}

void dspDrawImage(uint8_t handle, uint32_t x, uint32_t y, DSP_Image_t* img)
{

}

//
//
//

void dspRotate(uint8_t handle, uint8_t angle_code)
{
	if (!handle) return;
	if (!dsp[handle-1].init) return;
	if (dsp[handle-1].init->driver->rotate) dsp[handle-1].init->driver->rotate(dsp[handle-1].init->drvSettings,angle_code);
}

void dspDrawElipse(uint8_t handle, uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad)
{
    uint32_t ax = getArcX(x,1,xrad);
    uint32_t ay = getArcY(y,1,yrad);

    for (uint16_t t = 2; t < 360; t++)
    {
        if (t%90)
        {
            uint32_t bx = getArcX(x,t,xrad);
            uint32_t by = getArcY(y,t, yrad);
            dspDrawLine(handle, ax, ay, bx, by);
            ax = bx;
            ay = by;
        }
    }
}

void dspDrawCyrcle(uint8_t handle, uint32_t x, uint32_t y, uint32_t rad)
{
    dspDrawElipse(handle, x,y,rad,rad);
}

void dspFillElipse(uint8_t handle, uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad)
{
    for (int t = 1; t < 180; t++)
    {
        if (t%90)
        {
            uint32_t ax = getArcX(x,180+t,xrad);
            uint32_t ay = getArcY(y,180+t,yrad);

            uint32_t bx = getArcX(x,180-t,xrad);
            uint32_t by = getArcY(y,180-t,yrad);
            dspDrawLine(handle, ax,ay,bx,by);
        }
    }
}

void dspFillCyrcle(uint8_t handle, uint32_t x, uint32_t y, uint32_t rad)
{
    dspFillElipse(handle, x,y,rad,rad);
}

void dspDrawArc(uint8_t handle, uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad, uint16_t start, uint32_t stop)
{
    uint32_t ax = getArcX(x,start,xrad);
    uint32_t ay = getArcY(y,start,yrad);

    for (uint16_t t = start+1; t <= stop; t++)
    {
        if (t%90)
        {
            uint32_t bx = getArcX(x,t,xrad);
            uint32_t by = getArcY(y,t, yrad);
            dspDrawLine(handle, ax, ay, bx, by);
            ax = bx;
            ay = by;
        }
    }
}

void dspFillArc(uint8_t handle, uint32_t x, uint32_t y, uint32_t xrad, uint32_t yrad, uint16_t start, uint32_t stop)
{
    uint16_t axis = (stop-start)/2;
    uint32_t ax,ay,bx,by;
    for (uint16_t t = 0; t < axis; t++)
    {
        ax = getArcX(x,start+t,xrad);
        ay = getArcY(y,start+t,yrad);

        bx = getArcX(x,stop-t,xrad);
        by = getArcY(y,stop-t,yrad);

        dspDrawLine(handle, ax,ay,bx,by);
    }
}

void dspFillRectangleGradient(uint8_t handle, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint8_t orientation, uint32_t startColor, uint32_t stopColor)
{
    if (!handle) return;
    if (!dsp[handle-1].init) return;
    if (!dsp[handle-1].init->driver->decodeColor) return;
    if (!dsp[handle-1].init->driver->bitPerPixel) return;
    if (!dsp[handle-1].init->driver->rgbColor) return;
    if (dsp[handle-1].init->driver->bitPerPixel(dsp[handle-1].init->drvSettings) < 12) return;


    int32_t dx = (x2-x1);
    int32_t dy = (y2-y1);

    int8_t  sx = (dx == 0)?0:((dx>0)?1:-1);
    int8_t  sy = (dy == 0)?0:((dy>0)?1:-1);

    int32_t x = x1;
    int32_t y = y1;

    //uint16_t line_len = (orientation == DSP_GRADIENT_HORIZONTAL)?ABS(y2-y1):ABS(x2-x1);
    uint16_t lines_count = (orientation == DSP_GRADIENT_HORIZONTAL)?ABS(x2-x1):ABS(y2-y1);
    RGB_Color_t start_struct = dsp[handle-1].init->driver->decodeColor(dsp[handle-1].init->drvSettings,startColor);
    RGB_Color_t stop_struct = dsp[handle-1].init->driver->decodeColor(dsp[handle-1].init->drvSettings,stopColor);

    uint16_t line_red = start_struct.red*100;
    uint16_t line_green = start_struct.green*100;
    uint16_t line_blue = start_struct.blue*100;

    int16_t dr = stop_struct.red - start_struct.red;
    int16_t dg = stop_struct.green - start_struct.green;
    int16_t db = stop_struct.blue - start_struct.blue;

    int16_t sr = dr*100/lines_count;
    int16_t sg = dg*100/lines_count;
    int16_t sb = db*100/lines_count;

    uint32_t color = dsp[handle-1].foreColor;
    uint8_t end = 0;

        while (!end)
        {
            dsp[handle-1].foreColor = dsp[handle-1].init->driver->rgbColor(dsp[handle-1].init->drvSettings,line_red/100, line_green/100, line_blue/100, 255);

            if (orientation == DSP_GRADIENT_HORIZONTAL)
            {
                end = (x == x2)?1:0;
                dspDrawLine(handle, x, y1, x, y2);
                x += sx;
            }
            else
            {
                end = (y == y2)?1:0;
                dspDrawLine(handle, x1, y, x2, y);
                y += sy;
            }

            line_red += sr;
            line_green += sg;
            line_blue += sb;
            //HAL_Delay(3);
        }

    dsp[handle-1].foreColor = color;
}

