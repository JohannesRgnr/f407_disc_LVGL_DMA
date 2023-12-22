#ifndef __ST7789V_H
#define __ST7789V_H
#include "fonts.h"
#include "main.h"

/* choose a Hardware SPI port to use. */
#define ST7789V_SPI_PORT hspi1
extern SPI_HandleTypeDef ST7789V_SPI_PORT;

/* choose whether use DMA or not */
// #define USE_DMA 

/* If u need CS control, comment below*/
//#define CFG_NO_CS

/**
 * Pin assignment:
 * CS   ------> PA4
 * DC   ------> PA1
 * RST  ------> PA3
 */
/* Pin connection*/
#define ST7789V_RST_PORT GPIOA
#define ST7789V_RST_PIN  GPIO_PIN_3
#define ST7789V_DC_PORT  GPIOA
#define ST7789V_DC_PIN   GPIO_PIN_1

#ifndef CFG_NO_CS
#define ST7789V_CS_PORT  GPIOA
#define ST7789V_CS_PIN   GPIO_PIN_4
#endif

/* If u need Backlight control, uncomment below */
//#define BLK_PORT
//#define BLK_PIN


/*
 * Comment one to use another.
 * 3 parameters can be choosed
 * 135x240(0.96 inch) & 240x240(1.3inch) & 170x320(1.9inch)
 * X_SHIFT & Y_SHIFT are used to adapt different display's resolution
 */

/* Choose a type you are using */
//#define USING_135X240
//#define USING_240X240
//#define USING_170X320
#define USING_240X320


/* Choose a display rotation you want to use: (0-3) */
//#define ST7789V_ROTATION 0
//#define ST7789V_ROTATION 1
#define ST7789V_ROTATION 3				
//#define ST7789V_ROTATION 3

#ifdef USING_135X240

    #if ST7789V_ROTATION == 0
        #define ST7789V_WIDTH 135
        #define ST7789V_HEIGHT 240
        #define X_SHIFT 53
        #define Y_SHIFT 40
    #endif

    #if ST7789V_ROTATION == 1
        #define ST7789V_WIDTH 240
        #define ST7789V_HEIGHT 135
        #define X_SHIFT 40
        #define Y_SHIFT 52
    #endif

    #if ST7789V_ROTATION == 2
        #define ST7789V_WIDTH 135
        #define ST7789V_HEIGHT 240
        #define X_SHIFT 52
        #define Y_SHIFT 40
    #endif

    #if ST7789V_ROTATION == 3
        #define ST7789V_WIDTH 240
        #define ST7789V_HEIGHT 135
        #define X_SHIFT 40
        #define Y_SHIFT 53
    #endif

#endif

#ifdef USING_240X240

    #define ST7789V_WIDTH 240
    #define ST7789V_HEIGHT 240

		#if ST7789V_ROTATION == 0
			#define X_SHIFT 0
			#define Y_SHIFT 80
		#elif ST7789V_ROTATION == 1
			#define X_SHIFT 80
			#define Y_SHIFT 0
		#elif ST7789V_ROTATION == 2
			#define X_SHIFT 0
			#define Y_SHIFT 0
		#elif ST7789V_ROTATION == 3
			#define X_SHIFT 0
			#define Y_SHIFT 0
		#endif

#endif

#ifdef USING_170X320

	#if ST7789V_ROTATION == 0
        #define ST7789V_WIDTH 170
        #define ST7789V_HEIGHT 320
        #define X_SHIFT 35
        #define Y_SHIFT 0
    #endif

    #if ST7789V_ROTATION == 1
        #define ST7789V_WIDTH 320
        #define ST7789V_HEIGHT 170
        #define X_SHIFT 0
        #define Y_SHIFT 35
    #endif

    #if ST7789V_ROTATION == 2
        #define ST7789V_WIDTH 170
        #define ST7789V_HEIGHT 320
        #define X_SHIFT 35
        #define Y_SHIFT 0
    #endif

    #if ST7789V_ROTATION == 3
        #define ST7789V_WIDTH 320
        #define ST7789V_HEIGHT 170
        #define X_SHIFT 0
        #define Y_SHIFT 35
    #endif

#endif


#ifdef USING_240X320

	#if ST7789V_ROTATION == 0
        #define ST7789V_WIDTH 240
        #define ST7789V_HEIGHT 320
        #define X_SHIFT 0
        #define Y_SHIFT 0
    #endif

    #if ST7789V_ROTATION == 1
        #define ST7789V_WIDTH 320
        #define ST7789V_HEIGHT 240
        #define X_SHIFT 0
        #define Y_SHIFT 0
    #endif

    #if ST7789V_ROTATION == 2
        #define ST7789V_WIDTH 240
        #define ST7789V_HEIGHT 320
        #define X_SHIFT 0
        #define Y_SHIFT 0
    #endif

    #if ST7789V_ROTATION == 3
        #define ST7789V_WIDTH 320
        #define ST7789V_HEIGHT 240
        #define X_SHIFT 0
        #define Y_SHIFT 0
    #endif

#endif

/**
 *Color of pen
 *If you want to use another color, you can choose one in RGB565 format.
 */

#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define GRAY        0X8430
#define BRED        0XF81F
#define GRED        0XFFE0
#define GBLUE       0X07FF
#define BROWN       0XBC40
#define BRRED       0XFC07
#define DARKBLUE    0X01CF
#define LIGHTBLUE   0X7D7C
#define GRAYBLUE    0X5458

#define LIGHTGREEN  0X841F
#define LGRAY       0XC618
#define LGRAYBLUE   0XA651
#define LBBLUE      0X2B12

/* Control Registers and constant codes */
#define ST7789V_NOP     0x00
#define ST7789V_SWRESET 0x01
#define ST7789V_RDDID   0x04
#define ST7789V_RDDPM   0x0A
#define ST7789V_RDDMADCTL   0x0B

#define ST7789V_SLPIN   0x10
#define ST7789V_SLPOUT  0x11
#define ST7789V_PTLON   0x12
#define ST7789V_NORON   0x13

#define ST7789V_INVOFF  0x20
#define ST7789V_INVON   0x21
#define ST7789V_DISPOFF 0x28
#define ST7789V_DISPON  0x29
#define ST7789V_CASET   0x2A
#define ST7789V_RASET   0x2B
#define ST7789V_RAMWR   0x2C
#define ST7789V_RAMRD   0x2E

#define ST7789V_PTLAR   0x30
#define ST7789V_COLMOD  0x3A
#define ST7789V_MADCTL  0x36

#define ST7789V_PORCTRL 0xB2
#define ST7789V_GCTRL   0xB7
#define ST7789V_VCOMS   0xB7

#define ST7789V_LCMCTRL 0xC0
#define ST7789V_VDVVRHEN    0xC2
#define ST7789V_VRHS    0xC3
#define ST7789V_VDVSET  0xC4
#define ST7789V_FRCTLR2 0xC6

#define ST7789V_PWCTRL1 0xD0
#define ST7789V_PVGAMCTRL   0xE0
#define ST7789V_NVGAMCTRL   0xE1

/**
 * Memory Data Access Control Register (0x36H)
 * MAP:     D7  D6  D5  D4  D3  D2  D1  D0
 * param:   MY  MX  MV  ML  RGB MH  -   -
 *
 */

/* Page Address Order ('0': Top to Bottom, '1': the opposite) */
#define ST7789V_MADCTL_MY  0x80
/* Column Address Order ('0': Left to Right, '1': the opposite) */
#define ST7789V_MADCTL_MX  0x40
/* Page/Column Order ('0' = Normal Mode, '1' = Reverse Mode) */
#define ST7789V_MADCTL_MV  0x20
/* Line Address Order ('0' = LCD Refresh Top to Bottom, '1' = the opposite) */
#define ST7789V_MADCTL_ML  0x10
/* RGB/BGR Order ('0' = RGB, '1' = BGR) */
#define ST7789V_MADCTL_RGB 0x00

#define ST7789V_RDID1   0xDA
#define ST7789V_RDID2   0xDB
#define ST7789V_RDID3   0xDC
#define ST7789V_RDID4   0xDD

/* Advanced options */
#define ST7789V_COLOR_MODE_16bit 0x55    //  RGB565 (16bit)
#define ST7789V_COLOR_MODE_18bit 0x66    //  RGB666 (18bit)

/* Basic operations */
#define ST7789V_RST_Clr() HAL_GPIO_WritePin(ST7789V_RST_PORT, ST7789V_RST_PIN, GPIO_PIN_RESET)
#define ST7789V_RST_Set() HAL_GPIO_WritePin(ST7789V_RST_PORT, ST7789V_RST_PIN, GPIO_PIN_SET)

#define ST7789V_DC_Clr() HAL_GPIO_WritePin(ST7789V_DC_PORT, ST7789V_DC_PIN, GPIO_PIN_RESET)
#define ST7789V_DC_Set() HAL_GPIO_WritePin(ST7789V_DC_PORT, ST7789V_DC_PIN, GPIO_PIN_SET)
#ifndef CFG_NO_CS
#define ST7789V_Select() HAL_GPIO_WritePin(ST7789V_CS_PORT, ST7789V_CS_PIN, GPIO_PIN_RESET)
#define ST7789V_UnSelect() HAL_GPIO_WritePin(ST7789V_CS_PORT, ST7789V_CS_PIN, GPIO_PIN_SET)
#else
#define ST7789V_Select() asm("nop")
#define ST7789V_UnSelect() asm("nop")
#endif

#define ABS(x) ((x) > 0 ? (x) : -(x))

/* Basic functions. */
void ST7789V_Init(void);
void ST7789V_Send_Data_DMA(uint16_t x, uint16_t y, uint16_t x_end, uint16_t y_end, uint8_t *p);
void ST7789V_WriteData(uint8_t *buff, size_t buff_size);
void ST7789V_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ST7789V_SetRotation(uint8_t m);
void ST7789V_Fill_Color(uint16_t color);
void ST7789V_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7789V_Fill(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, uint16_t color);
void ST7789V_DrawPixel_4px(uint16_t x, uint16_t y, uint16_t color);

/* Graphical functions. */
void ST7789V_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void ST7789V_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void ST7789V_DrawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void ST7789V_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data);
void ST7789V_InvertColors(uint8_t invert);

/* Text functions. */
void ST7789V_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);
void ST7789V_WriteString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor);

/* Extented Graphical functions. */
void ST7789V_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7789V_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color);
void ST7789V_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color);
void ST7789V_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

/* Command functions */
void ST7789V_TearEffect(uint8_t tear);

/* Simple test function. */
void ST7789V_Test(void);

#ifndef ST7789V_ROTATION
    #error You should at least choose a display rotation!
#endif

#endif
