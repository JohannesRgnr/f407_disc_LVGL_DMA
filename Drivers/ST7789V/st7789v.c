/**
 * @file st7789v.c
 * @author johannes regnier
 * @brief Driver for ST7789V based LCD display. Adapted from https://github.com/Floyd-Fish/ST7789-STM32
 * @version 0.1
 * @date 2023-12-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "st7789v.h"
#include <stdlib.h>

#ifdef USE_DMA
#include <string.h>
uint16_t DMA_MIN_SIZE = 16;
/* If you're using DMA, then u need a "framebuffer" to store datas to be displayed.
 * If your MCU don't have enough RAM, please avoid using DMA(or set 5 to 1).
 * And if your MCU have enough RAM(even larger than full-frame size),
 * Then you can specify the framebuffer size to the full resolution below.
 */
 #define HOR_LEN 	5	//	Alse mind the resolution of your screen!
uint16_t disp_buf[ST7789V_WIDTH * HOR_LEN];
#endif

/**
 * @brief 
 * 
 * @param cmd 
 */
static void ST7789V_WriteCommand(uint8_t cmd)
{
	ST7789V_Select();
	ST7789V_DC_Clr();
	HAL_SPI_Transmit(&ST7789V_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);
	ST7789V_UnSelect();
}

/**
 * @brief Write data to ST7789V controller
 * @param buff -> pointer of data buffer
 * @param buff_size -> size of the data buffer
 * @return none
 */
void ST7789V_WriteData(uint8_t *buff, size_t buff_size)
{
	ST7789V_Select();

	ST7789V_DC_Set();

	// split data in small chunks because HAL can't send more than 64K at once

	while (buff_size > 0) {
		uint16_t chunk_size = buff_size > 65535 ? 65535 : buff_size;
		HAL_SPI_Transmit(&ST7789V_SPI_PORT, buff, chunk_size, 10);
		// HAL_SPI_Transmit_DMA(&ST7789V_SPI_PORT, buff, chunk_size);
		buff += chunk_size;
		buff_size -= chunk_size;
	}
	ST7789V_UnSelect();

}
/**
 * @brief Write data to ST7789V controller, simplify for 8bit data.
 * data -> data to write
 * @return none
 */
static void ST7789V_WriteSmallData(uint8_t data)
{
	 ST7789V_Select();
	ST7789V_DC_Set();
	HAL_SPI_Transmit(&ST7789V_SPI_PORT, &data, sizeof(data), HAL_MAX_DELAY);
	 ST7789V_UnSelect();
}

/**
 * @brief Set the rotation direction of the display
 * @param m -> rotation parameter(please refer it in ST7789V.h)
 * @return none
 */
void ST7789V_SetRotation(uint8_t m)
{
	ST7789V_WriteCommand(ST7789V_MADCTL);	// MADCTL
	switch (m) {
	case 0:
		ST7789V_WriteSmallData(ST7789V_MADCTL_MX | ST7789V_MADCTL_MY | ST7789V_MADCTL_RGB);
		break;
	case 1:
		ST7789V_WriteSmallData(ST7789V_MADCTL_MY | ST7789V_MADCTL_MV | ST7789V_MADCTL_RGB);
		break;
	case 2:
		ST7789V_WriteSmallData(ST7789V_MADCTL_RGB);
		break;
	case 3:
		ST7789V_WriteSmallData(ST7789V_MADCTL_MX | ST7789V_MADCTL_MV | ST7789V_MADCTL_RGB);
		break;
	default:
		break;
	}
}

/**
 * @brief Set address of DisplayWindow
 * @param xi&yi -> coordinates of window
 * @return none
 */
void ST7789V_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	// ST7789V_Select();
	uint16_t x_start = x0 + X_SHIFT, x_end = x1 + X_SHIFT;
	uint16_t y_start = y0 + Y_SHIFT, y_end = y1 + Y_SHIFT;
	
	/* Column Address set */
	ST7789V_WriteCommand(ST7789V_CASET); 
	{
		uint8_t data[] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
		ST7789V_WriteData(data, sizeof(data));
	}

	/* Row Address set */
	ST7789V_WriteCommand(ST7789V_RASET);
	{
		uint8_t data[] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
		ST7789V_WriteData(data, sizeof(data));
	}
	/* Write to RAM */
	ST7789V_WriteCommand(ST7789V_RAMWR);
	// ST7789V_UnSelect();
}

/**
 * @brief Initialize ST7789V controller
 * @param none
 * @return none
 */
void ST7789V_Init(void)
{
	// #ifdef USE_DMA
	// 	memset(disp_buf, 0, sizeof(disp_buf));
	// #endif
	
	HAL_Delay(50);
	ST7789V_DC_Set();
	HAL_Delay(50);
	ST7789V_RST_Clr();
    HAL_Delay(25);
    ST7789V_RST_Set();
	HAL_Delay(25);
    
	ST7789V_WriteCommand(ST7789V_MADCTL); 
	ST7789V_WriteSmallData(0x00); 

	ST7789V_WriteCommand(ST7789V_COLMOD); 
	ST7789V_WriteSmallData(0x05);

	ST7789V_WriteCommand(ST7789V_INVON); 

	ST7789V_WriteCommand(ST7789V_CASET);
	ST7789V_WriteSmallData(0x00);
	ST7789V_WriteSmallData(0x00);
	ST7789V_WriteSmallData(0x01);
	ST7789V_WriteSmallData(0x3F);

	ST7789V_WriteCommand(ST7789V_RASET);
	ST7789V_WriteSmallData(0x00);
	ST7789V_WriteSmallData(0x00);
	ST7789V_WriteSmallData(0x00);
	ST7789V_WriteSmallData(0xEF);

	ST7789V_WriteCommand(ST7789V_PORCTRL);
	ST7789V_WriteSmallData(0x0C);
	ST7789V_WriteSmallData(0x0C);
	ST7789V_WriteSmallData(0x00);
	ST7789V_WriteSmallData(0x33);
	ST7789V_WriteSmallData(0x33);

	

	ST7789V_WriteCommand(ST7789V_GCTRL);
	ST7789V_WriteSmallData(0x35); 

	ST7789V_WriteCommand(ST7789V_VCOMS);
	ST7789V_WriteSmallData(0x1F);

	ST7789V_WriteCommand(ST7789V_LCMCTRL);
	ST7789V_WriteSmallData(0x2C);

	ST7789V_WriteCommand(ST7789V_VDVVRHEN);
	ST7789V_WriteSmallData(0x01);

	ST7789V_WriteCommand(ST7789V_VRHS);
	ST7789V_WriteSmallData(0x12);   

	ST7789V_WriteCommand(ST7789V_VDVSET);
	ST7789V_WriteSmallData(0x20);

	ST7789V_WriteCommand(ST7789V_FRCTLR2);
	ST7789V_WriteSmallData(0x0F); 

	ST7789V_WriteCommand(ST7789V_PWCTRL1);
	ST7789V_WriteSmallData(0xA4);
	ST7789V_WriteSmallData(0xA1);

	ST7789V_WriteCommand(ST7789V_PVGAMCTRL);
	ST7789V_WriteSmallData(0xD0);
	ST7789V_WriteSmallData(0x08);
	ST7789V_WriteSmallData(0x11);
	ST7789V_WriteSmallData(0x08);
	ST7789V_WriteSmallData(0x0C);
	ST7789V_WriteSmallData(0x15);
	ST7789V_WriteSmallData(0x39);
	ST7789V_WriteSmallData(0x33);
	ST7789V_WriteSmallData(0x50);
	ST7789V_WriteSmallData(0x36);
	ST7789V_WriteSmallData(0x13);
	ST7789V_WriteSmallData(0x14);
	ST7789V_WriteSmallData(0x29);
	ST7789V_WriteSmallData(0x2D);

	ST7789V_WriteCommand(ST7789V_NVGAMCTRL);
	ST7789V_WriteSmallData(0xD0);
	ST7789V_WriteSmallData(0x08);
	ST7789V_WriteSmallData(0x10);
	ST7789V_WriteSmallData(0x08);
	ST7789V_WriteSmallData(0x06);
	ST7789V_WriteSmallData(0x06);
	ST7789V_WriteSmallData(0x39);
	ST7789V_WriteSmallData(0x44);
	ST7789V_WriteSmallData(0x51);
	ST7789V_WriteSmallData(0x0B);
	ST7789V_WriteSmallData(0x16);
	ST7789V_WriteSmallData(0x14);
	ST7789V_WriteSmallData(0x2F);
	ST7789V_WriteSmallData(0x31);

    ST7789V_WriteCommand (ST7789V_INVON);		//	Inversion ON
	ST7789V_WriteCommand (ST7789V_SLPOUT);	//	Out of sleep mode
  	ST7789V_WriteCommand (ST7789V_NORON);		//	Normal Display on
  	ST7789V_WriteCommand (ST7789V_DISPON);	//	Main screen turned on	

	HAL_Delay(50);
	ST7789V_SetRotation(3);
	// ST7789V_Fill_Color(BLACK);				//	Fill with Black.
}


/* memset function */
/* Thanks to  BTurc.2 for the fix*/
/*https://community.st.com/t5/stm32-mcus-products/using-spi-with-dma/td-p/571681/page/2*/
void MemsetBuffer(uint16_t *buf, uint16_t data, uint32_t size)
{
	for (uint32_t contador = 0; contador < size; contador++)
	{			
		buf[contador] = data;
	}
}



/**
 * @brief Fill the DisplayWindow with single color
 * @param color -> color to Fill with
 * @return none
 */
void ST7789V_Fill_Color(uint16_t color)
{
	uint16_t i, convert_color;
	ST7789V_SetAddressWindow(0, 0, ST7789V_WIDTH - 1, ST7789V_HEIGHT - 1);
	ST7789V_Select();

	convert_color = (uint16_t)(color << 8) | (color >> 8);

	#ifdef USE_DMA
		for (i = 0; i < ST7789V_HEIGHT / HOR_LEN; i++)
		{
			MemsetBuffer(disp_buf, convert_color, sizeof(disp_buf));
			ST7789V_WriteData(disp_buf, sizeof(disp_buf));
		}
	#else
		uint16_t j;
		for (i = 0; i < ST7789V_WIDTH; i++)
				for (j = 0; j < ST7789V_HEIGHT; j++) {
					uint8_t data[] = {color >> 8, color & 0xFF};
					ST7789V_WriteData(data, sizeof(data));
				}
	#endif
	ST7789V_UnSelect();
}

/**
 * @brief Draw a Pixel
 * @param x&y -> coordinate to Draw
 * @param color -> color of the Pixel
 * @return none
 */
void ST7789V_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	if ((x < 0) || (x >= ST7789V_WIDTH) ||
		 (y < 0) || (y >= ST7789V_HEIGHT))	return;
	
	ST7789V_SetAddressWindow(x, y, x, y);
	uint8_t data[] = {color >> 8, color & 0xFF};
	ST7789V_Select();
	ST7789V_WriteData(data, sizeof(data));
	ST7789V_UnSelect();
}

/**
 * @brief Fill an Area with single color
 * @param xSta&ySta -> coordinate of the start point
 * @param xEnd&yEnd -> coordinate of the end point
 * @param color -> color to Fill with
 * @return none
 */
void ST7789V_Fill(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, uint16_t color)
{
	if ((xEnd < 0) || (xEnd >= ST7789V_WIDTH) ||
		 (yEnd < 0) || (yEnd >= ST7789V_HEIGHT))	return;
	//ST7789V_Select();
	uint16_t i, j;
	ST7789V_SetAddressWindow(xSta, ySta, xEnd, yEnd);
	for (i = ySta; i <= yEnd; i++)
		for (j = xSta; j <= xEnd; j++) {
			uint8_t data[] = {color >> 8, color & 0xFF};
			ST7789V_WriteData(data, sizeof(data));
		}
	//ST7789V_UnSelect();
}

/**
 * @brief Draw a big Pixel at a point
 * @param x&y -> coordinate of the point
 * @param color -> color of the Pixel
 * @return none
 */
void ST7789V_DrawPixel_4px(uint16_t x, uint16_t y, uint16_t color)
{
	if ((x <= 0) || (x > ST7789V_WIDTH) ||
		 (y <= 0) || (y > ST7789V_HEIGHT))	return;
	ST7789V_Select();
	ST7789V_Fill(x - 1, y - 1, x + 1, y + 1, color);
	ST7789V_UnSelect();
}

/**
 * @brief Draw a line with single color
 * @param x1&y1 -> coordinate of the start point
 * @param x2&y2 -> coordinate of the end point
 * @param color -> color of the line to Draw
 * @return none
 */
void ST7789V_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
        uint16_t color) {
	uint16_t swap;
    uint16_t steep = ABS(y1 - y0) > ABS(x1 - x0);
    if (steep) {
		swap = x0;
		x0 = y0;
		y0 = swap;

		swap = x1;
		x1 = y1;
		y1 = swap;
        //_swap_int16_t(x0, y0);
        //_swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
		swap = x0;
		x0 = x1;
		x1 = swap;

		swap = y0;
		y0 = y1;
		y1 = swap;
        //_swap_int16_t(x0, x1);
        //_swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = ABS(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            ST7789V_DrawPixel(y0, x0, color);
        } else {
            ST7789V_DrawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

/**
 * @brief Draw a Rectangle with single color
 * @param xi&yi -> 2 coordinates of 2 top points.
 * @param color -> color of the Rectangle line
 * @return none
 */
void ST7789V_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	ST7789V_Select();
	ST7789V_DrawLine(x1, y1, x2, y1, color);
	ST7789V_DrawLine(x1, y1, x1, y2, color);
	ST7789V_DrawLine(x1, y2, x2, y2, color);
	ST7789V_DrawLine(x2, y1, x2, y2, color);
	ST7789V_UnSelect();
}

/** 
 * @brief Draw a circle with single color
 * @param x0&y0 -> coordinate of circle center
 * @param r -> radius of circle
 * @param color -> color of circle line
 * @return  none
 */
void ST7789V_DrawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	ST7789V_Select();
	ST7789V_DrawPixel(x0, y0 + r, color);
	ST7789V_DrawPixel(x0, y0 - r, color);
	ST7789V_DrawPixel(x0 + r, y0, color);
	ST7789V_DrawPixel(x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7789V_DrawPixel(x0 + x, y0 + y, color);
		ST7789V_DrawPixel(x0 - x, y0 + y, color);
		ST7789V_DrawPixel(x0 + x, y0 - y, color);
		ST7789V_DrawPixel(x0 - x, y0 - y, color);

		ST7789V_DrawPixel(x0 + y, y0 + x, color);
		ST7789V_DrawPixel(x0 - y, y0 + x, color);
		ST7789V_DrawPixel(x0 + y, y0 - x, color);
		ST7789V_DrawPixel(x0 - y, y0 - x, color);
	}
	ST7789V_UnSelect();
}

/**
 * @brief Draw an Image on the screen
 * @param x&y -> start point of the Image
 * @param w&h -> width & height of the Image to Draw
 * @param data -> pointer of the Image array
 * @return none
 */
void ST7789V_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data)
{
	if ((x >= ST7789V_WIDTH) || (y >= ST7789V_HEIGHT))
		return;
	if ((x + w - 1) >= ST7789V_WIDTH)
		return;
	if ((y + h - 1) >= ST7789V_HEIGHT)
		return;

	ST7789V_Select();
	ST7789V_SetAddressWindow(x, y, x + w - 1, y + h - 1);
	ST7789V_WriteData((uint8_t *)data, sizeof(uint16_t) * w * h);
	ST7789V_UnSelect();
}

/**
 * @brief Invert Fullscreen color
 * @param invert -> Whether to invert
 * @return none
 */
void ST7789V_InvertColors(uint8_t invert)
{
	ST7789V_Select();
	ST7789V_WriteCommand(invert ? 0x21 /* INVON */ : 0x20 /* INVOFF */);
	ST7789V_UnSelect();
}

/** 
 * @brief Write a char
 * @param  x&y -> cursor of the start point.
 * @param ch -> char to write
 * @param font -> fontstyle of the string
 * @param color -> color of the char
 * @param bgcolor -> background color of the char
 * @return  none
 */
void ST7789V_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
	uint32_t i, b, j;
	ST7789V_Select();
	ST7789V_SetAddressWindow(x, y, x + font.width - 1, y + font.height - 1);

	for (i = 0; i < font.height; i++) {
		b = font.data[(ch - 32) * font.height + i];
		for (j = 0; j < font.width; j++) {
			if ((b << j) & 0x8000) {
				uint8_t data[] = {color >> 8, color & 0xFF};
				ST7789V_WriteData(data, sizeof(data));
			}
			else {
				uint8_t data[] = {bgcolor >> 8, bgcolor & 0xFF};
				ST7789V_WriteData(data, sizeof(data));
			}
		}
	}
	ST7789V_UnSelect();
}

/** 
 * @brief Write a string 
 * @param  x&y -> cursor of the start point.
 * @param str -> string to write
 * @param font -> fontstyle of the string
 * @param color -> color of the string
 * @param bgcolor -> background color of the string
 * @return  none
 */
void ST7789V_WriteString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor)
{
	ST7789V_Select();
	while (*str) {
		if (x + font.width >= ST7789V_WIDTH) {
			x = 0;
			y += font.height;
			if (y + font.height >= ST7789V_HEIGHT) {
				break;
			}

			if (*str == ' ') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}
		ST7789V_WriteChar(x, y, *str, font, color, bgcolor);
		x += font.width;
		str++;
	}
	ST7789V_UnSelect();
}

/** 
 * @brief Draw a filled Rectangle with single color
 * @param  x&y -> coordinates of the starting point
 * @param w&h -> width & height of the Rectangle
 * @param color -> color of the Rectangle
 * @return  none
 */
void ST7789V_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	ST7789V_Select();
	uint8_t i;

	/* Check input parameters */
	if (x >= ST7789V_WIDTH ||
		y >= ST7789V_HEIGHT) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= ST7789V_WIDTH) {
		w = ST7789V_WIDTH - x;
	}
	if ((y + h) >= ST7789V_HEIGHT) {
		h = ST7789V_HEIGHT - y;
	}

	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		ST7789V_DrawLine(x, y + i, x + w, y + i, color);
	}
	ST7789V_UnSelect();
}

/** 
 * @brief Draw a Triangle with single color
 * @param  xi&yi -> 3 coordinates of 3 top points.
 * @param color ->color of the lines
 * @return  none
 */
void ST7789V_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color)
{
	ST7789V_Select();
	/* Draw lines */
	ST7789V_DrawLine(x1, y1, x2, y2, color);
	ST7789V_DrawLine(x2, y2, x3, y3, color);
	ST7789V_DrawLine(x3, y3, x1, y1, color);
	ST7789V_UnSelect();
}

/** 
 * @brief Draw a filled Triangle with single color
 * @param  xi&yi -> 3 coordinates of 3 top points.
 * @param color ->color of the triangle
 * @return  none
 */
void ST7789V_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color)
{
	ST7789V_Select();
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
			yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
			curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	}
	else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	}
	else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay) {
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	}
	else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		ST7789V_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
	ST7789V_UnSelect();
}

/** 
 * @brief Draw a Filled circle with single color
 * @param x0&y0 -> coordinate of circle center
 * @param r -> radius of circle
 * @param color -> color of circle
 * @return  none
 */
void ST7789V_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	ST7789V_Select();
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	ST7789V_DrawPixel(x0, y0 + r, color);
	ST7789V_DrawPixel(x0, y0 - r, color);
	ST7789V_DrawPixel(x0 + r, y0, color);
	ST7789V_DrawPixel(x0 - r, y0, color);
	ST7789V_DrawLine(x0 - r, y0, x0 + r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7789V_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
		ST7789V_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

		ST7789V_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
		ST7789V_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
	}
	ST7789V_UnSelect();
}


/**
 * @brief Open/Close tearing effect line
 * @param tear -> Whether to tear
 * @return none
 */
void ST7789V_TearEffect(uint8_t tear)
{
	ST7789V_Select();
	ST7789V_WriteCommand(tear ? 0x35 /* TEON */ : 0x34 /* TEOFF */);
	ST7789V_UnSelect();
}


/** 
 * @brief A Simple test function for ST7789V
 * @param  none
 * @return  none
 */
void ST7789V_Test(void)
{
	ST7789V_SetRotation(3);
	ST7789V_Fill_Color(WHITE);
	HAL_Delay(1000);
	ST7789V_WriteString(10, 20, "Speed Test", Font_11x18, RED, WHITE);
	HAL_Delay(1000);
	ST7789V_Fill_Color(CYAN);
    HAL_Delay(500);
	ST7789V_Fill_Color(RED);
    HAL_Delay(500);
	ST7789V_Fill_Color(BLUE);
    HAL_Delay(500);
	ST7789V_Fill_Color(GREEN);
    HAL_Delay(500);
	ST7789V_Fill_Color(YELLOW);
    HAL_Delay(500);
	ST7789V_Fill_Color(BROWN);
    HAL_Delay(500);
	ST7789V_Fill_Color(DARKBLUE);
    HAL_Delay(500);
	ST7789V_Fill_Color(MAGENTA);
    HAL_Delay(500);
	ST7789V_Fill_Color(LIGHTGREEN);
    HAL_Delay(500);
	ST7789V_Fill_Color(LGRAY);
    HAL_Delay(500);
	ST7789V_Fill_Color(LBBLUE);
    HAL_Delay(500);
	ST7789V_Fill_Color(WHITE);
	HAL_Delay(500);

	ST7789V_WriteString(10, 10, "Font test.", Font_16x26, GBLUE, WHITE);
	ST7789V_WriteString(10, 50, "Hello Steve!", Font_7x10, RED, WHITE);
	ST7789V_WriteString(10, 75, "Hello Steve!", Font_11x18, YELLOW, WHITE);
	ST7789V_WriteString(10, 100, "Hello Steve!", Font_16x26, MAGENTA, WHITE);
	HAL_Delay(1000);

	ST7789V_Fill_Color(BLUE);
	ST7789V_WriteString(10, 10, "Rect./Line.", Font_11x18, YELLOW, BLACK);
	ST7789V_DrawRectangle(30, 30, 100, 100, WHITE);
	HAL_Delay(1000);

	ST7789V_Fill_Color(RED);
	ST7789V_WriteString(10, 10, "Filled Rect.", Font_11x18, YELLOW, BLACK);
	ST7789V_DrawFilledRectangle(30, 30, 50, 50, WHITE);
	HAL_Delay(1000);

	ST7789V_Fill_Color(RED);
	ST7789V_WriteString(10, 10, "Circle.", Font_11x18, YELLOW, BLACK);
	ST7789V_DrawCircle(60, 60, 25, WHITE);
	HAL_Delay(1000);

	ST7789V_Fill_Color(RED);
	ST7789V_WriteString(10, 10, "Filled Cir.", Font_11x18, YELLOW, BLACK);
	ST7789V_DrawFilledCircle(60, 60, 25, WHITE);
	HAL_Delay(1000);

	ST7789V_Fill_Color(RED);
	ST7789V_WriteString(10, 10, "Triangle", Font_11x18, YELLOW, BLACK);
	ST7789V_DrawTriangle(30, 30, 30, 70, 60, 40, WHITE);
	HAL_Delay(1000);

	ST7789V_Fill_Color(RED);
	ST7789V_WriteString(10, 10, "Filled Tri", Font_11x18, YELLOW, BLACK);
	ST7789V_DrawFilledTriangle(30, 30, 30, 70, 60, 40, WHITE);
	HAL_Delay(1000);

	//	If FLASH cannot storage anymore datas, please delete codes below.
	ST7789V_Fill_Color(BLACK);
	ST7789V_DrawImage(0, 0, 128, 128, (uint16_t *)saber);
	HAL_Delay(3000);
}


void ST7789V_Test2(void)
{
	float x = 40, y = 40;
	uint16_t oldx=40, oldy=40;
	ST7789V_SetRotation(3);
	ST7789V_Fill_Color(BLACK);
	while (1)
	{
		
		//ST7789V_DrawFilledCircle(oldx, oldy, 10, BLACK);
		ST7789V_DrawCircle(oldx, oldy, 10, BLACK);
		ST7789V_DrawCircle(x, y, 10, WHITE);
		oldx = x;
		oldy = y;
		x +=  (y < ST7789V_WIDTH) ? 0.2 : -0.2;
		y +=  (x < ST7789V_HEIGHT) ? 0.2 : -0.2;
		// y += 0.2;
		
	}
}


static void ST7789V_SPI_Send_DMA(uint8_t *data, uint16_t size)
{
	HAL_SPI_Transmit_DMA(&ST7789V_SPI_PORT,data,size);
}

void ST7789V_Send_Data_DMA(uint16_t x, uint16_t y, uint16_t x_end, uint16_t y_end, uint8_t *p){
	ST7789V_SetAddressWindow(x, y, x_end, y_end);
	ST7789V_DC_Set();
	ST7789V_SPI_Send_DMA(p,(x_end - x + 1) * (y_end - y + 1) * 2);
}