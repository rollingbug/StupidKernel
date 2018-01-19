/*
 *****************************************************************************
 * My Stupid Kernel
 *
 *
 * File Name:   floppy.h
 * File Desc:   function to initial and control floppy driver
 * Author:      Y.S.Kuo
 * Date:        20130119
 *****************************************************************************
 */
#ifndef _FLOPPY_H_
#define _FLOPPY_H_

#include "../types.h"

#define FLOPPY_DRIVE_NUM    4

#define FLOPPY_RETRY_MAX    60

#define FLOPPY_READ         0x01
#define FLOPPY_WRITE        0x02

#define FLOPPY_DMA_CHANNEL  2

#define FLOPPY_ISR_NUM      38

/* registers of floppy drive */
enum FLOPPY_REGISTERS{
	FLOPPY_SRA  = 0x3F0,		/* status register A, read-only */
	FLOPPY_SRB	= 0x3F1,		/* status register B, read-only */
	FLOPPY_DOR	= 0x3F2,		/* digital output register */
	FLOPPY_TDR	= 0x3F3,		/* tape drive register */
	FLOPPY_MSR	= 0x3F4,		/* main status register, read-only */
	FLOPPY_DSR	= 0x3F4,		/* data rate select register, write only */
	FLOPPY_DATA	= 0x3F5,		/* data fifo */
	FLOPPY_DIR	= 0x3F7,		/* digital input register, read only */
	FLOPPY_CCR	= 0x3F7		    /* configuration control register, write only */
};

/* floppy command */
enum FLOPPY_COMMAND{
	FLOPPY_CMD_READ_TRACK 			= 2,
	FLOPPY_CMD_SPECIFY				= 3,
	FLOPPY_CMD_SENSE_DRIVE_STATUS	= 4,
	FLOPPY_CMD_WRITE_DATA			= 5,
	FLOPPY_CMD_READ_DATA			= 6,
	FLOPPY_CMD_RECALLIBRATE		    = 7,
	FLOPPY_CMD_SENSE_INTERRUPT		= 8,
	FLOPPY_CMD_WRITE_DELETED_DATA	= 9,
	FLOPPY_CMD_READ_ID				= 10,
	FLOPPY_CMD_READ_DELETED_DATA	= 12,
	FLOPPY_CMD_FORMAT_TRACK		    = 13,
	FLOPPY_CMD_SEEK				    = 15,
	FLOPPY_CMD_VERSION				= 16,
	FLOPPY_CMD_SCAN_EQUAL			= 17,
	FLOPPY_CMD_PERPENDDICULAR_MODE	= 18,
	FLOPPY_CMD_CONFIGURE			= 19,
	FLOPPY_CMD_LOCK				    = 20,
	FLOPPY_CMD_VERIFY				= 22,
	FLOPPY_CMD_SCAN_LOW_OR_EQUAL	= 25,
	FLOPPY_CMD_SCAN_HIGH_OR_EQUAL	= 29
};

typedef struct floppy_info{
    INT8U *type_name;
    INT8U heads;
    INT8U tracks;
    INT8U sectors;
    INT16U sector_size;
    INT8U gap_len;
}FLOPPY_INFO;

typedef struct floppy_drive{
    INT8U type;
    FLOPPY_INFO *info;
}FLOPPY_DRIVE;

INT8S floppy_read(INT8U drive_sel, INT32U logic_sector_num, INT16U sectors_to_read, void *p_buffer);
INT8S floppy_write(INT8U drive_sel, INT8U logic_sector_num, INT16U sectors_to_write, void *p_buffer);
INT8S floppy_init();

#endif
