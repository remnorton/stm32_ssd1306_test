/*
 * dsp_driver.h
 *
 *  Created on: 08.09.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */


#ifndef DSP_DRIVER_H
#define DSP_DRIVER_H
#include "hal_includes.h"
#include "fonts_lib.h"
#include "intmath.h"

typedef struct
{
    uint16_t x;
    uint16_t y;
}DSP_POINT_t;

typedef struct
{
    DSP_POINT_t top_left;
    DSP_POINT_t bot_right;
}DSP_RECT_t;


typedef struct
{
    uint32_t width;
    uint32_t height;
    uint8_t* data;
}DSP_Bitmap_t;

typedef struct
{
	uint32_t width;
	uint32_t height;
	uint8_t	 bit_per_pixel;
	uint8_t* data;
}DSP_Image_t;

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
}RGB_Color_t;

//
//
//

enum
{
    DSP_COLOR_Black = 0x00,
    DSP_COLOR_White = 0xFFFFFFFF
};

enum
{
    DSP_GRADIENT_HORIZONTAL = 0,
    DSP_GRADIENT_VERTICAL
};

enum
{
	DSP_Rotate_0 = 0,
	DSP_Rotate_90,
	DSP_Rotate_180,
	DSP_Rotate_270
};

typedef union
{
    uint16_t color;
    struct
    {
        uint16_t b : 5;
        uint16_t g : 6;
        uint16_t r : 5;
    }rgb;
}RGB16_ColorStruct_t;

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct
{
    uint8_t     (*init)(void*);
    void		(*deInit)(void*);
    uint16_t    (*getId)(void*);
    uint16_t    (*screenWidth)(void*);
    uint16_t    (*screenHeight)(void*);
    void        (*clearScreen)(void*,uint32_t);
    void        (*setInverse)(void*,uint8_t);
    void        (*switchOn)(void*,uint8_t);
    void        (*setBrightnes)(void*,uint32_t);
    void        (*drawPixel)(void*,uint32_t, uint32_t, uint32_t);
    void        (*drawLine)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    void        (*drawRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    void        (*fillRectangle)(void*,uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    uint16_t    (*drawChar)(void*,uint32_t, uint32_t, Font_type_t*, uint8_t, uint32_t);
    void        (*drawString)(void*,uint32_t, uint32_t, Font_type_t*, uint8_t*, uint32_t, uint8_t);
    void        (*sendCommands)(void*,uint8_t*, uint8_t);
    void		(*sendData)(void*,uint8_t*, uint16_t);
    void        (*pushData)(void*);
    uint32_t    (*rgbColor)(void*,uint8_t, uint8_t, uint8_t, uint8_t);
    uint32_t    (*pixelColor)(void*,uint32_t, uint32_t);
    uint32_t    (*bitPerPixel)(void*);
    void        (*drawBitmap)(void*,uint32_t, uint32_t, DSP_Bitmap_t*, uint32_t/*color*/);
    void		(*drawImage)(void*,uint32_t, uint32_t, DSP_Image_t*);
    RGB_Color_t (*decodeColor)(void*,uint32_t);
    void		(*rotate)(void*,uint8_t);

}DSP_DriverDef_t;


 typedef struct
 {
     DSP_DriverDef_t*    driver;
     void*               drvSettings;
 }DSPInitStruct_t;


//
//List of existig drivers
//

extern DSP_DriverDef_t ssd1306_driver;
extern DSP_DriverDef_t ili9325_driver;

#ifdef __cplusplus
}
#endif

#endif // DSP_DRIVER_H
