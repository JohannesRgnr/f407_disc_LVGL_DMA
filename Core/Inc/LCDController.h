/**
 * @file LCDController.h
 *
 */

/*set this value to "1" to enable content*/
#if 1

#ifndef LCDCONTROLLER_H
#define LCDCONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lvgl.h"


/*********************
 *      DEFINES
 *********************/
#define MY_DISP_HOR_RES     320
#define MY_DISP_VER_RES     240
#define LV_HOR_RES_MAX      320
#define LV_VER_RES_MAX      240

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/* Initialize low level display driver */
void lv_port_disp_init(void);

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void);

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LCDCONTROLLER_H*/

#endif /*Disable/Enable content*/
