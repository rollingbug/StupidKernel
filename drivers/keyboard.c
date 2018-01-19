/*
 *****************************************************************************
 * My Stupid Kernel
 *
 *
 * File Name:   keyboard.c
 * File Desc:   keyboard driver
 * Author:      Y.S.Kuo
 * Date:        20130119
 *****************************************************************************
 */

#include "keyboard.h"
#include "../kernel.h"
#include "../interrupt/interrupt.h"

static INT8U keyboard_key_make;
static INT8U keyboard_key_multi;
static INT8U keyboard_key_shift;
static INT8U keyboard_key_ctrl;
static INT8U keyboard_key_alt;
static INT8U keyboard_key_num;
static INT8U keyboard_key_caps;
static INT8U keyboard_key_scroll;

const static INT8U keyboard_scode_no_shift[] =
{
    0x0,    '1',    '2',    '3',    '4',    '5',    '6',    '7',    '8',    '9',
    '0',    '-',    '=',   '\r',   '\t',    'q',    'w',    'e',    'r',    't',
    'y',    'u',    'i',    'o',    'p',    '[',    ']',   '\n',    0x0,    'a',
    's',    'd',    'f',    'g',    'h',    'j',    'k',    'l',    ';',   '\'',
    '`',    0x0,   '\\',    'z',    'x',    'c',    'v',    'b',    'n',    'm',
    ',',    '.',    '/',    0x0,    '*',    0x0,    0x0,    0x0,    0x0,    0x0,
    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
    '7',    '8',    '9',    '-',    '4',    '5',    '6',    '+',    '1',    '2',
    '3',    '0',    '.',    0x0,    0x0,    0x0,    0x0,    0x0
};

const static INT8U keyboard_scode_with_shift[] =
{
    0x0,    '!',    '@',    '#',    '$',    '%',    '^',    '&',    '*',    '(',
    ')',    '_',    '+',   '\r',   '\t',    'Q',    'W',    'E',    'R',    'T',
    'Y',    'U',    'I',    'O',    'P',    '{',    '}',   '\n',    0x0,    'A',
    'S',    'D',    'F',    'G',    'H',    'J',    'K',    'L',    ':',   '\"',
    '~',    0x0,    '|',    'Z',    'X',    'C',    'V',    'B',    'N',    'M',
    '<',    '>',    '?',    0x0,    '*',    0x0,    0x0,    0x0,    0x0,    0x0,
    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,    0x0,
    '7',    '8',    '9',    '-',    '4',    '5',    '6',    '+',    '1',    '2',
    '3',    '0',    '.',    0x0,    0x0,    0x0,    0x0,    0x0
};

/* keyboard interrupt hander */
void keyboard_handler()
{
    INT8U scan_code;

    /* get the scan code from keyboard controller */
    scan_code = inb(0x60);

    if(scan_code > 0x58 && scan_code != KEYBOARD_KEY_MULTI){
        scan_code -= 0x80;
        keyboard_key_make = 0;           /* button up */
    }
    else
        keyboard_key_make = 1;           /* button down */

    switch(scan_code){
        case KEYBOARD_KEY_LCTRL:
            keyboard_key_ctrl = keyboard_key_make;
            return;
        case KEYBOARD_KEY_LSHFT:
        case KEYBOARD_KEY_RSHFT:
            keyboard_key_shift = keyboard_key_make;
            return;
        case KEYBOARD_KEY_LALT:
            keyboard_key_alt = keyboard_key_make;
            return;
        case KEYBOARD_KEY_CAPS:
            keyboard_key_caps = keyboard_key_make;
            return;
        case KEYBOARD_KEY_NUM:
            keyboard_key_num = keyboard_key_make;
            return;
        case KEYBOARD_KEY_SCROLL:
            keyboard_key_scroll = keyboard_key_make;
            return;
        case KEYBOARD_KEY_TAB:
        case KEYBOARD_KEY_BKSP:
            return;
        default:
            break;
    }

    if(scan_code == KEYBOARD_KEY_MULTI){
        keyboard_key_multi = 1;
        return;
    }
    else if(keyboard_key_multi == 1){        /* function key */
        keyboard_key_multi = 0;

        switch(scan_code){
            case KEYBOARD_KEY_UP:
            case KEYBOARD_KEY_LEFT:
            case KEYBOARD_KEY_DOWN:
            case KEYBOARD_KEY_RIGHT:
            case KEYBOARD_KEY_F1:
            case KEYBOARD_KEY_F2:
            case KEYBOARD_KEY_F3:
            case KEYBOARD_KEY_F4:
            case KEYBOARD_KEY_F5:
            case KEYBOARD_KEY_F6:
            case KEYBOARD_KEY_F7:
            case KEYBOARD_KEY_F8:
            case KEYBOARD_KEY_F9:
            case KEYBOARD_KEY_F10:
            case KEYBOARD_KEY_F11:
            case KEYBOARD_KEY_F12:
            case KEYBOARD_KEY_ESC:
                return;
            case KEYBOARD_KEY_RALT:
                keyboard_key_alt = keyboard_key_make;
                return;
            case KEYBOARD_KEY_RCTRL:
                keyboard_key_ctrl = keyboard_key_make;
                return;
            case KEYBOARD_KEY_KP_ENTER:
            case KEYBOARD_KEY_KP_SLASH:
                break;
            default:
                return;
        }
    }

    if(keyboard_key_make == 1){
        if(keyboard_key_shift == 1)
            console_put_char(keyboard_scode_with_shift[scan_code - 1]);
        else
            console_put_char(keyboard_scode_no_shift[scan_code - 1]);
    }
}

void keyboard_init()
{
    keyboard_key_make = 0;
    keyboard_key_multi = 0;
    keyboard_key_shift = 0;
    keyboard_key_ctrl = 0;
    keyboard_key_alt = 0;
    keyboard_key_num = 0;
    keyboard_key_caps = 0;
    keyboard_key_scroll = 0;

    KERNEL_MSG(FALSE, TRUE, "initializing keyboard ...\n");

    /* setup keyboard isr handler */
    interrupt_set_isr_handler(KEYBOARD_ISR_NUM, keyboard_handler);
}
