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
#include "console.h"

static INT16U console_x_pos;
static INT16U console_y_pos;

void console_set_cursor(INT16U x_pos, INT16U y_pos)
{
    INT16U pos;

    pos = x_pos + (y_pos * CONSOLE_ROW);

    /* set cursor position */
    outb(0x3D4, 0x0F);
    outb(0x3D5, (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (pos >> 8) & 0xFF);
}

void console_clr_screen()
{
	INT16U x_pos;
	INT16U y_pos;
	INT16S *p_console_mem;

	p_console_mem = (INT16S *)CONSOLE_MEM;

	for(x_pos = 0; x_pos < CONSOLE_ROW; x_pos++){
		for(y_pos = 0; y_pos < CONSOLE_COLUMN; y_pos++){
			*p_console_mem = ' ' | (CONSOLE_BLACK << 12 | CONSOLE_WHITE << 8);
			p_console_mem++;
		}
	}

	console_x_pos = 0;
	console_y_pos = 0;

	console_set_cursor(0, 0);
}

void console_scroll_up()
{
    INT16U x_pos;
	INT16U y_pos;
    INT16S *p_console_mem;

	p_console_mem = (INT16S *)CONSOLE_MEM;

    for(y_pos = 0; y_pos < CONSOLE_COLUMN - 1; y_pos++){
        memcpy((void *)p_console_mem,
               (void *)(p_console_mem + CONSOLE_ROW),
               CONSOLE_LINE_SIZE);

        p_console_mem += CONSOLE_ROW;
    }

    for(x_pos = 0; x_pos < CONSOLE_ROW; x_pos++){
        *p_console_mem = ' ' | (CONSOLE_BLACK << 12 | CONSOLE_WHITE << 8);
        p_console_mem++;
    }
}

void console_put_char(INT8S chr)
{
	INT16S *p_console_mem;

    /* new line */
	if(chr == '\n'){
		console_x_pos = 0;

		if(console_y_pos == CONSOLE_COLUMN - 1)
			console_scroll_up();
		else
            console_y_pos++;
	}
	else{
		if(console_x_pos == CONSOLE_ROW){
			console_x_pos = 0;
			console_y_pos++;
		}

		if(console_y_pos == CONSOLE_COLUMN){
		    console_x_pos = 0;
			console_y_pos--;
			console_scroll_up();
		}

		p_console_mem = (INT16S *)(CONSOLE_MEM + (console_y_pos * CONSOLE_ROW + console_x_pos) * 2);
		*p_console_mem = chr | (CONSOLE_BLACK << 12 | CONSOLE_WHITE << 8);

		console_x_pos++;
	}

	console_set_cursor(console_x_pos, console_y_pos);
}

void console_put_str(INT8S *str)
{
	INT32U str_len;

	str_len = 0;
	while(str[str_len] != 0){
		console_put_char(str[str_len]);
		str_len++;
	}
}

void console_printf(INT8S *format, ...)
{
	INT8S chr;
	INT8S *str;
	INT8S **arg;
	INT8S buffer[16];

	arg = (INT8S **)&(format);
	arg++;

	memset((void *)buffer, 0, 16);
	while((chr = *format++) != 0){
		if(chr != '%')
			console_put_char(chr);
		else{
			chr = *format++;
			switch(chr){
				case 'd':
				case 'o':
				case 'x':
				case 'u':
					itoa(*((INT32S *)(arg++)), buffer, chr);
					console_put_str(buffer);
					break;
				case 's':
					str = (INT8S *)(*arg++);
					if(!str)
						str = "(null)";
					console_put_str(str);
					break;
				default:
					console_put_char(*((INT32S *)(arg++)));
					break;
			}
		}
	}
}

void console_init()
{
	console_clr_screen();
}
