#ifndef __FONTS_LIB_H
#define __FONTS_LIB_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif


#pragma pack(1)
typedef struct
{
    uint16_t offset;    //! offset in bitmap table
    uint8_t width;      //! width of symbol
    uint8_t height;     //! height of symbol
    uint8_t trimed;     //! quantity of empty lines, removed from bottom
}Font_Char_info_t;
#pragma pack()

typedef struct
{
    char name[16];              //! name of font, may be empty
    uint8_t* bitmaps;           //! table of bitmaps of symbols
    Font_Char_info_t* descr;    //! table of descriptors of symbols. May be NULL for monospace fonts
    uint8_t width;              //! width of symbol. Ignored if table of descriptors are defined
    uint8_t height;             //! untrimmed symbols height
    uint8_t start;              //! code of first symbol in table
    uint8_t stop;               //! code of last symbol in table
    uint8_t spacing;            //! spacing between symbols in line
}Font_type_t;


enum
{
    DSP_TextAllignLeft = 0,
    DSP_TextAllignRight,
    DSP_TextAllignCenter
};


#ifdef __cplusplus
}
#endif


extern Font_type_t font_Courier_New_8pt;
extern Font_type_t font_Courier_New_10pt;


uint16_t calc_pix_len(Font_type_t* font, uint8_t* text, uint16_t ch_len);

#endif /* __FONTS_LIB_H */


