/*
 *****************************************************************************
 * My Stupid Kernel
 *
 *
 * File Name:   lib.c
 * File Desc:   generic kernel function library
 * Author:      Y.S.Kuo
 * Date:        20130119
 *****************************************************************************
 */
#include "lib.h"

static INT8S ascii_table[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

void itoa(INT32S value, INT8S *str, INT32S base)
{
	INT32S div;
	INT8S tmp;
	INT8S *p_chr;
	INT8S *p_head;
	INT32U in_value;

	p_chr = str;
	p_head = str;

	switch(base){
		case 2:
		case 8:
		case 10:
		case 16:
			div = base;
			break;
		case 'x':
			div = 16;
			break;
		case 'o':
			div = 8;
			break;
		default:
			div = 10;
			break;
	}

	if(value < 0 && div == 10 &&  base != 'u'){
		*p_chr++ = '-';
		p_head++;
		in_value = -(value);
	}
	else
		in_value = value;

	do{
		*p_chr++ = ascii_table[in_value % div];
		in_value /= div;
	}while(in_value != 0);

	*p_chr = 0;
	p_chr--;
	while(p_head < p_chr){
		tmp = *p_head;
		*p_head = *p_chr;
		*p_chr = tmp;

		p_chr--;
		p_head++;
	}
}

void *memset(void *mem, INT32S value, INT32U num)
{
    void *ptr;

    ptr = mem;

	if(ptr == NULL || num == 0)
		return NULL;

	while(num){
	    if((INT32U)ptr & 0x3 == 0)          /* align 4 */
            break;

		*((INT8S *)ptr) = (INT8S)value;
		ptr++;
		num--;
	}

	if(value == 0){                         /* access 4bytes */
		while(num > 4){
			*((INT32S *)ptr) = value;
			ptr += 4;
			num -= 4;
		}
	}

	while(num){
		*((INT8S *)ptr) = (INT8S)value;
		ptr++;
		num--;
	}

	return mem;
}

void *memcpy(void *dst_ptr, void *src_ptr, INT32U len)
{
    INT8S *p_dst;
    INT8S *p_src;

    p_dst = (INT8S *)dst_ptr;
    p_src = (INT8S *)src_ptr;

    while(len-- > 0)
        *p_dst++ = *p_src++;

    return dst_ptr;
}

INT32S memcmp(void *p1, void *p2, INT32U num)
{
    INT8U *p_mem1;
    INT8U *p_mem2;
    INT32U cnt;

    p_mem1 = (INT8U *)p1;
    p_mem2 = (INT8U *)p2;
    cnt = 0;

    while(cnt < num){
        if(*p_mem1 - *p_mem2 != 0)
            return (*p_mem1 - *p_mem2);

        cnt++;
        p_mem1++;
        p_mem2++;
    }

    return 0;
}

INT32S islower(INT8S chr)
{
    if(chr >= 'a' && chr <= 'z')
        return TRUE;
    else
        return FALSE;
}

INT32S isupper(INT8S chr)
{
    if(chr >= 'A' && chr <= 'Z')
        return TRUE;
    else
        return FALSE;
}

INT32S toupper(INT8S chr)
{
    if(islower(chr) == TRUE)
        chr -= 32;

    return chr;
}

INT32S tolower(INT8S chr)
{
    if(isupper(chr) == TRUE)
        chr += 32;

    return chr;
}

INT8S *strupr(INT8S *str)
{
    INT8S *tmp;

    tmp = str;

    while(*tmp != '\0'){
        *tmp = toupper(*tmp);
        tmp++;
    }

    return str;
}

INT8S *strcpy(INT8S *p_dst, INT8S *p_src)
{
    INT8S *p_tmp;

    if(p_dst == NULL || p_src == NULL)
        return NULL;

    p_tmp = p_dst;

    while(*p_src != '\0')
        *p_dst++ = *p_src++;

    return p_dst;
}

INT32U strlen(INT8S *p_str){
    INT32U len;

    len = 0;
    while(*p_str++ != '\0')
        len++;

    return len;
}

void outb(INT16U port, INT8U value)
{
	asm("outb %%al, %%dx"
		:
		: "d" (port), "a" (value)
		:
	);
}

INT8U inb(INT16U port)
{
	INT8U res;

	asm("inb %%dx, %%al"
		:"=a" (res)
        :"dx" (port)
	);

	return res;
}

