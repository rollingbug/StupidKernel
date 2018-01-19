/*
 *****************************************************************************
 * My Stupid Kernel
 *
 *
 * File Name:   keyboard.h
 * File Desc:   keyboard driver
 * Author:      Y.S.Kuo
 * Date:        20130119
 *****************************************************************************
 */
#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "../types.h"

#define KEYBOARD_ISR_NUM    33

#define KEYBOARD_KEY_ESC         0x01
#define KEYBOARD_KEY_BKSP        0x0E
#define KEYBOARD_KEY_TAB         0x0F
#define KEYBOARD_KEY_ENTER       0x1C
#define KEYBOARD_KEY_KP_ENTER    0x1C
#define KEYBOARD_KEY_LCTRL       0x1D
#define KEYBOARD_KEY_RCTRL       0x1D
#define KEYBOARD_KEY_LSHFT       0x2A
#define KEYBOARD_KEY_KP_SLASH    0x35
#define KEYBOARD_KEY_RSHFT       0x36
#define KEYBOARD_KEY_LALT        0x38
#define KEYBOARD_KEY_RALT        0x38
#define KEYBOARD_KEY_SPACE       0x39
#define KEYBOARD_KEY_CAPS        0x3A
#define KEYBOARD_KEY_F1          0x3B
#define KEYBOARD_KEY_F2          0x3C
#define KEYBOARD_KEY_F3          0x3D
#define KEYBOARD_KEY_F4          0x3E
#define KEYBOARD_KEY_F5          0x3F
#define KEYBOARD_KEY_F6          0x40
#define KEYBOARD_KEY_F7          0x41
#define KEYBOARD_KEY_F8          0x42
#define KEYBOARD_KEY_F9          0x43
#define KEYBOARD_KEY_F10         0x44
#define KEYBOARD_KEY_NUM         0x45
#define KEYBOARD_KEY_SCROLL      0x46
#define KEYBOARD_KEY_UP          0x48
#define KEYBOARD_KEY_LEFT        0x4B
#define KEYBOARD_KEY_RIGHT       0x4D
#define KEYBOARD_KEY_DOWN        0x50
#define KEYBOARD_KEY_F11         0x57
#define KEYBOARD_KEY_F12         0x58
#define KEYBOARD_KEY_MULTI       0xE0

void keyboard_init();

#endif
