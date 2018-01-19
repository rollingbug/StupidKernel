/*
 *****************************************************************************
 * My Stupid Kernel
 *
 *
 * File Name:   console.c
 * File Desc:   functions for console operating
 * Author:      Y.S.Kuo
 * Date:        20130119
 *****************************************************************************
 */
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "../types.h"

#define CONSOLE_MEM		    0xB8000
#define CONSOLE_ROW		    80
#define CONSOLE_COLUMN	    25
#define CONSOLE_SIZE	    CONSOLE_ROW * CONSOLE_COLUMN
#define CONSOLE_LINE_SIZE   (CONSOLE_ROW * 2)

enum CONSOLE_COLOR{
	CONSOLE_BLACK = 0,
	CONSOLE_BLUE,
	CONSOLE_GREEN,
	CONSOLE_CYAN,
	CONSOLE_RED,
	CONSOLE_MAGENTA,
	CONSOLE_BROWN,
	CONSOLE_LIGHT_GRAY,
	CONSOLE_DARK_GRAY,
	CONSOLE_LIGHT_BLUE,
	CONSOLE_LIGHT_GREEN,
	CONSOLE_LIGHT_CYAN,
	CONSOLE_LIGHT_RED,
	CONSOLE_LIGHT_MAGENTA,
	CONSOLE_YELLOW,
	CONSOLE_WHITE
};

void console_set_cursor(INT16U x_pos, INT16U y_pos);
void console_clr_screen();
void console_scroll_up();
void console_put_char(INT8S chr);
void console_put_str(INT8S *str);
void console_printf(INT8S *format, ... );
void console_init();

#endif
