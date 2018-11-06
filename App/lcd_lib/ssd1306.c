/*
 * ssd1306.cpp
 *
 *  Created on: 07.09.2018
 *		Author: Rudenko Roman (aka Rem Norton)
 *
 */
#include <string.h>
#include <stdlib.h>
#include "ssd1306.h"

#define _SSD1306_ID_                (uint16_t)0x1306
#define _SSD1306_SCREEN_WIDTH_      (uint16_t)128
#define _SSD1306_BPP_               (uint32_t)0x01

static SSD1306_INIT_t* instance = 0;
static uint8_t* screen_buffer = 0;
static uint16_t buf_size = 0;

static uint8_t init_sequence[] = {
                            0xAE,       //display off,
                            0x20,       //Set Memory Addressing Mode
                            0x00,       //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
                            0x00,       //set low column address
                            0x10,       //set high column address
                            0x40,       //set start line address
                            0xC8,       //Set COM Output Scan Direction
							0xB0,		//Set page 0
                            0x81,0xFF,  //set contrast control register
                            0xA1,       //set segment re-map 0 to 127
                            0xA6,       //set normal display
                            0xA8,0x3F,  //set multiplex ratio(1 to 64)
                            0xA4,       //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
                            0xD3,0x00,  //set display offset
                            0xD5,       //set display clock divide ratio/oscillator frequency
                            0xF0,       //set divide ratio
                            0xD9,0x22,  //set pre-charge period
                            0xDA,0x12,  //set com pins hardware configuration
                            0xDB,0x20,  //set vcomh
                            0x8D,0x14,  //set DC-DC enable
                            0xAF,       //turn on SSD1306 panel
};
//
//
//

void ssd1306_send_commands(uint8_t* commands, uint8_t len)
{
	if (!instance) return;
    HAL_I2C_Mem_Write(instance->i2c, instance->dsp_addr, 0x00, 1, commands, len, 10);
}

void ssd1306_sendData(uint8_t* data, uint16_t len)
{
	if (!instance) return;
	HAL_I2C_Mem_Write(instance->i2c, instance->dsp_addr, 0x40, 1, data, sizeof(len), 100);
}

uint16_t ssd1306_get_screen_width()
{
    return _SSD1306_SCREEN_WIDTH_;
}

uint16_t ssd1306_get_screen_height()
{
	if (!instance) return 0;
    return instance->height;
}

void ssd1306_push_screen()
{
	if (!instance) return;
    HAL_I2C_Mem_Write(instance->i2c, instance->dsp_addr,0x40,1,screen_buffer,buf_size,100);
}

void ssd1306_switch_on(uint8_t active)
{
	if (!instance) return;
    uint8_t cmd = active?0xAF:0xAE;//--turn on SSD1306 panel
    ssd1306_send_commands(&cmd, 1);
}

uint8_t ssd1306_init(void* init)
{
	if (!init) return 0;
	instance = (SSD1306_INIT_t*)init;
	buf_size = _SSD1306_SCREEN_WIDTH_*(instance->height)/8;
	screen_buffer = (uint8_t*)calloc(1,buf_size);
	if (!screen_buffer)
	{
		instance = 0;
		return 0;
	}
	ssd1306_send_commands(init_sequence, sizeof(init_sequence));
	return 1;
}

void ssd1306_deInit()
{
	if (!instance) return;
	ssd1306_switch_on(0);
	if (screen_buffer)
	{
		free(screen_buffer);
		screen_buffer = 0;
		buf_size = 0;
	}
	instance = 0;
}

uint16_t ssd1306_get_id()
{
	if (!instance) return 0;
    return _SSD1306_ID_;
}

void ssd1306_clear_screen(uint32_t color)
{
	if (!instance) return;
    memset(screen_buffer, (color == DSP_COLOR_Black)?0x00:0xff, buf_size);
}

void ssd1306_set_inverse(uint8_t inverse)
{
	if (!instance) return;
    uint8_t cmd = inverse?0xA7:0xA6;
    ssd1306_send_commands(&cmd,1);
}

void ssd1306_set_brightnes(uint32_t level)
{
	if (!instance) return;
    if (level > 255) level = 255;
    uint8_t cmd[2] = {0x81,(uint8_t)(level & 0xff)};
    ssd1306_send_commands(cmd, sizeof(cmd));
}

void ssd1306_draw_pixel(uint32_t x, uint32_t y, uint32_t color)
{
	if (!instance) return;
    if ((x >= ssd1306_get_screen_width()) || (y >= ssd1306_get_screen_height())) return;
    if (color == DSP_COLOR_White)
        screen_buffer[x + (y / 8) * ssd1306_get_screen_width()] |= 1 << (y % 8);
    else
        screen_buffer[x + (y / 8) * ssd1306_get_screen_width()] &= ~(1 << (y % 8));
}

void ssd1306_draw_bitmap(uint32_t x, uint32_t y, DSP_Bitmap_t* bmp, uint32_t color)
{
	if (!instance) return;
    if (!bmp) return;
    uint32_t width = bmp->width;
    while (width % 8) width++;

    for (uint32_t by = 0; by < bmp->height; by++)
    {
        for (uint32_t bx = 0; bx < bmp->width; bx++)
        {
            if (bmp->data[bx/8+(by*width/8)] & (0x80 >> (bx%8))) ssd1306_draw_pixel(x+bx, y+by, color);
        }
    }
}

uint16_t ssd1306_draw_char(uint32_t x, uint32_t y, Font_type_t* font, uint8_t sym, uint32_t color)
{
	if (!instance) return 0;
	if (!font) return 0;
	if (sym > font->stop) return 0;
	uint8_t vs = (font->descr)?1:0;
	Font_Char_info_t info = font->descr[(sym-font->start)];

	uint8_t sym_width = vs?info.width:font->width;
	uint8_t sym_height = vs?info.height:font->height;
	uint8_t byte_width = sym_width/8+((sym_width % 8)?1:0);

	uint16_t offset = vs?info.offset:((sym-font->start)*byte_width*font->height);

	if ( ((x+sym_width) > ssd1306_get_screen_width()) || ((y+font->height) > ssd1306_get_screen_height() )) return 0;
	uint16_t startY = vs?(font->height-font->descr[(sym-font->start)].trimed - sym_height):0;


	DSP_Bitmap_t bmp;
	bmp.height = sym_height;
	bmp.width = byte_width*8;
	uint16_t bmp_size = sym_height*byte_width;
	bmp.data = (uint8_t*)malloc(bmp_size);

	memcpy(bmp.data, font->bitmaps+offset,bmp_size);

	ssd1306_draw_bitmap(x,y+startY,&bmp,color);
	free(bmp.data);
	return sym_width+font->spacing;
}

uint32_t ssd1306_rgb_color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
{
    UNUSED(alpha);
    if (!instance) return 0;
    uint32_t res = (red+green+blue);
    return (res > 0)?1:0;
}

uint32_t ssd1306_pixel_color(uint32_t x, uint32_t y)
{
	if (!instance) return 0;
    if ((x > ssd1306_get_screen_width()) || (y > ssd1306_get_screen_height())) return DSP_COLOR_Black;
    if (screen_buffer[x + (y / 8) * ssd1306_get_screen_width()] & (1 << (y % 8)) ) return DSP_COLOR_White;
    return DSP_COLOR_Black;
}

uint32_t ssd1306_bit_per_pixel()
{
	if (!instance) return 0;
    return _SSD1306_BPP_;
}

RGB_Color_t ssd1306_decode_color(uint32_t color)
{
	if (!instance) return (RGB_Color_t){0,0,0,0};
    RGB_Color_t res = {0,0,0,255};
    if (color)
        res.green = res.blue = res.red = 255;
    return res;
}

void ssd1306_rotate(uint8_t angle_code)
{
	if (!instance) return;
	switch(angle_code)
	{
		case DSP_Rotate_0:
		{
			uint8_t cmd[] = {0xc8,0xa1,0xd3,0x00};
			ssd1306_send_commands(cmd, sizeof(cmd));
			break;
		}
		case DSP_Rotate_180:
		{
			uint8_t cmd[] = {0xc0,0xa0,0xd3,instance->yellow_lines};
			ssd1306_send_commands(cmd, sizeof(cmd));
			break;
		}
	}
}

//
//Driver definition
//

DSP_DriverDef_t ssd1306_driver = {
        ssd1306_init,                   //void        (*init)(void*);
		0,//void		(*deInit)();
        ssd1306_get_id,                 //uint16_t    (*getId)();
        ssd1306_get_screen_width,       //uint16_t    (*screenWidth)();
        ssd1306_get_screen_height,      //uint16_t    (*screenHeight)();
        ssd1306_clear_screen,           //void        (*clearScreen)(uint32_t);
        ssd1306_set_inverse,            //void        (*setInverse)(uint8_t);
        ssd1306_switch_on,              //void        (*switchOn)(uint8_t);
		ssd1306_set_brightnes,          //void        (*setBrightnes)(uint32_t);
        ssd1306_draw_pixel,             //void        (*drawPixel)(uint32_t, uint32_t, uint32_t);
        0,                              //void        (*drawLine)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
        0,                              //void        (*drawRectangle)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
        0,                              //void        (*fillRectangle)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
		ssd1306_draw_char,              //uint16_t    (*drawChar)(uint32_t, uint32_t, FONT_t*, uint8_t, uint32_t);
        0,                              //void        (*drawString)(uint32_t, uint32_t, FONT_t*, uint8_t*, uint32_t, uint8_t);
        ssd1306_send_commands,          //void        (*sendCommands)(uint8_t*, uint8_t);
		ssd1306_sendData,				//void		  (*sendData)(uint8_t*, uint16_t);
        ssd1306_push_screen,            //void        (*pushData)();
        ssd1306_rgb_color,              //uint32_t    (*rgbColor)(uint8_t, uint8_t, uint8_t, uint8_t);
        ssd1306_pixel_color,            //uint32_t    (*pixelColor)(uint32_t, uint32_t);
        ssd1306_bit_per_pixel,          //uint32_t    (*bitPerPixel)();
        ssd1306_draw_bitmap,            //void        (*drawBitmap)(uint32_t, uint32_t, DSP_Bitmap_t*);
		0,								//void		  (*drawImage)(uint32_t, uint32_t, DSP_Image_t*);
        ssd1306_decode_color,           //RGB_Color_t (*decodeColor)(uint32_t);
		ssd1306_rotate,					//void		  (*rotate)(uint8_t);
};
