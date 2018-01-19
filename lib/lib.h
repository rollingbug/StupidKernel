/*
 *****************************************************************************
 * My Stupid Kernel
 *
 *
 * File Name:   lib.h
 * File Desc:   generic kernel function library
 * Author:      Y.S.Kuo
 * Date:        20130119
 *****************************************************************************
 */
#ifndef _LIB_H_
#define _LIB_H_

#include "../types.h"

void itoa(INT32S value, INT8S *str, INT32S base);
void *memset(void *ptr, INT32S value, INT32U num);
void *memcpy(void *dst_ptr, void *src_ptr, INT32U len);
INT32S memcmp(void *p1, void *p2, INT32U num);
INT32S islower(INT8S chr);
INT32S isupper(INT8S chr);
INT32S toupper(INT8S chr);
INT32S tolower(INT8S chr);
INT8S *strupr(INT8S *str);
INT8S *strcpy(INT8S *p_dst, INT8S *p_src);
INT32U strlen(INT8S *p_str);
void outb(INT16U port, INT8U value);
INT8U inb(INT16U port);

#endif
