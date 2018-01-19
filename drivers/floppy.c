/*
 *****************************************************************************
 * My Stupid Kernel
 *
 *
 * File Name:   floppy.c
 * File Desc:   function to initial and control floppy driver
 * Author:      Y.S.Kuo
 * Date:        20130119
 *****************************************************************************
 */
#include "floppy.h"
#include "../kernel.h"
#include "../interrupt/interrupt.h"

#define FLOPPY_MSR_ACT_A        (1 << 0)
#define FLOPPY_MSR_ACT_B        (1 << 1)
#define FLOPPY_MSR_ACT_C        (1 << 2)
#define FLOPPY_MSR_ACT_D        (1 << 3)
#define FLOPPY_MSR_BUSY         (1 << 4)
#define FLOPPY_MSR_NO_DMA       (1 << 5)
#define FLOPPY_MSR_DIO          (1 << 6)
#define FLOPPY_MSR_MRQ          (1 << 7)

#define FLOPPY_DOR_DR0
#define FLOPPY_DOR_DR1
#define FLOPPY_DOR_ENABLE       (1 << 2)
#define FLOPPY_DOR_DMA          (1 << 3)
#define FLOPPY_DOR_MOTO_A       (1 << 4)
#define FLOPPY_DOR_MOTO_B       (1 << 5)
#define FLOPPY_DOR_MOTO_C       (1 << 6)
#define FLOPPY_DOR_MOTO_D       (1 << 7)

#define FLOPPY_DATARATE_500KB   0
#define FLOPPY_DATARATE_300KB   1
#define FLOPPY_DATARATE_250KB   2
#define FLOPPY_DATARATE_1MB     3

static volatile INT8U floppy_interrupt_flag;

static FLOPPY_DRIVE floppy_drive[FLOPPY_DRIVE_NUM] ={
    {0, NULL},
    {0, NULL},
    {0, NULL},
    {0, NULL}
};

static FLOPPY_INFO floppy_info[] = {
    {"NONE",            0,  0,  0,   0,    0},
    {"360KB 5.25\"",    2, 40,  9, 512, 0x2A},
    {"1.2MB 5.25\"",    2, 80, 15, 512, 0x2A},
    {"720KB 3.5\"",     2, 80,  9, 512, 0x1B},
    {"1.44MB 3.5\"",    2, 80, 18, 512, 0x1B},
    {"2.88MB 3.5\"",    2, 80, 36, 512, 0x1B}
};

/* floppy interrupt handler */
static void floppy_handler()
{
    floppy_interrupt_flag = TRUE;
}

/* send command to fdc */
static INT8S floppy_send_byte(INT8U cmd)
{
    INT32U timeout;

    timeout = 0;
    while(timeout++ < FLOPPY_RETRY_MAX){
        if((inb(FLOPPY_MSR) & FLOPPY_MSR_MRQ) != 0){
            outb(FLOPPY_DATA, cmd);
            return 0;
        }
    }

    return -1;
}

/* get result from fdc */
static INT16S floppy_get_byte()
{
    INT32U timeout;

    timeout = 0;
    while(timeout++ < FLOPPY_RETRY_MAX){
        if((inb(FLOPPY_MSR) & (FLOPPY_MSR_MRQ | FLOPPY_MSR_DIO)) != 0)
            return inb(FLOPPY_DATA);
    }

    return -1;
}

/* wait for floppy interrupt */
static void floppy_wait()
{
    while(1){
        if(floppy_interrupt_flag == TRUE)
            break;
    }

    floppy_interrupt_flag = FALSE;
}

/* check interrupt status */
static void floppy_sense_interrupt(INT16S *st0, INT16S *pcn)
{
    floppy_send_byte(FLOPPY_CMD_SENSE_INTERRUPT);

    *st0 = floppy_get_byte();
    *pcn = floppy_get_byte();
}

static void floppy_reset()
{
    INT16S st0;
    INT16S pcn;

    outb(FLOPPY_DOR, ~FLOPPY_DOR_ENABLE);
    outb(FLOPPY_DOR, (FLOPPY_DOR_DMA | FLOPPY_DOR_ENABLE));

    floppy_wait();
    floppy_sense_interrupt(&st0, &pcn);

    outb(FLOPPY_CCR, FLOPPY_DATARATE_500KB);
}

static void floppy_select(INT8U drive_sel)
{
    INT8U dor;

    dor = inb(FLOPPY_DOR);
    dor &= 0xFC;                        /* 11111100 */
    dor |= (drive_sel & 0x03);
    outb(FLOPPY_DOR, dor);
}

/* turn off the floppy drive motor */
static void floppy_motor_on(INT8U drive_sel)
{
    INT8U dor;

    dor = inb(FLOPPY_DOR);
    dor &= 0xFC;                        /* 11111100 */
    dor |= (drive_sel & 0x03);
    dor |= 0x10 << (drive_sel & 0x03);
    outb(FLOPPY_DOR, dor);
}

/* turn on the floppy drive motor */
static void floppy_motor_off(INT8U drive_sel)
{
    INT8U dor;

    dor = inb(FLOPPY_DOR);
    dor &= 0xFC;                        /* 11111100 */
    dor |= (drive_sel & 0x03);
    dor &= ~(0x10 << (drive_sel & 0x03));
    outb(FLOPPY_DOR, dor);
}

static INT8S floppy_recalibrate(INT8U drive_sel)
{
    INT16S st0;
    INT16S pcn;
    INT32U timeout;

    floppy_motor_on(drive_sel);

    timeout = 0;
    while(timeout++ < FLOPPY_RETRY_MAX){
        floppy_send_byte(FLOPPY_CMD_RECALLIBRATE);
        floppy_send_byte(drive_sel & 0x03);     /* 00000011 */
        floppy_wait();
        floppy_sense_interrupt(&st0, &pcn);

        if(st0 & (FLOPPY_MSR_DIO | FLOPPY_MSR_MRQ) != 0)
            continue;

        if(pcn == 0){
            floppy_motor_off(drive_sel);
            return 0;
        }
    }

    floppy_motor_off(drive_sel);

    return -1;
}

/* seek floppy track */
static INT8S floppy_seek(INT8U drive_sel, INT8U head, INT8U track)
{
    INT32U timeout;
    INT16S st0;
    INT16S pcn;

    floppy_motor_on(drive_sel);

    timeout = 0;
    head = (head & 0x01) << 2;              /* 00000001 */
    drive_sel &= 0x03;                      /* 00000011 */
    while(timeout++ < FLOPPY_RETRY_MAX){
        floppy_send_byte(FLOPPY_CMD_SEEK);
        floppy_send_byte(head | drive_sel);
        floppy_send_byte(track);
        floppy_wait();
        floppy_sense_interrupt(&st0, &pcn);

        if(st0 & (FLOPPY_MSR_DIO | FLOPPY_MSR_MRQ) != 0)
            continue;

        if(pcn == track){
            floppy_motor_off(drive_sel);
            return 0;
        }
    }

    floppy_motor_off(drive_sel);

    return -1;
}

/* convert logical sector number to physical floppy head, track and sector number */
static void floppy_get_position(INT8U drive_sel, INT32U logic_sector_num, INT8U *head, INT8U *track, INT8U *sector)
{
    *head = (logic_sector_num % (floppy_drive[drive_sel].info->heads * floppy_drive[drive_sel].info->sectors)) / floppy_drive[drive_sel].info->sectors;
    *track = logic_sector_num / (floppy_drive[drive_sel].info->heads * floppy_drive[drive_sel].info->sectors);
    *sector = logic_sector_num % floppy_drive[drive_sel].info->sectors + 1;
}

/* read/write floppy data block */
static INT8S floppy_rw_block(INT8U drive_sel, INT32U logic_sector_num, INT8U rw_mode)
{
    INT32U timeout;
    INT8S op_result;
    INT8U cmd;
    INT8U rw_error;
    INT8U head;
    INT8U track;
    INT8U sector;
    INT16S result_st0;
    INT16S result_st1;
    INT16S result_st2;
    INT16S result_track;
    INT16S result_head;
    INT16S result_sector;
    INT16S result_sector_size;

    if(floppy_drive[drive_sel].info == NULL || floppy_drive[drive_sel].type == 0)
        return -1;

    if(rw_mode == FLOPPY_READ)
        cmd = FLOPPY_CMD_READ_DATA | 0xC0;
    else if(rw_mode == FLOPPY_WRITE)
        cmd = FLOPPY_CMD_WRITE_DATA | 0xC0;
    else
        return -1;

    floppy_get_position(drive_sel, logic_sector_num, &head, &track, &sector);

    if(head > floppy_drive[drive_sel].info->heads||
       track > floppy_drive[drive_sel].info->tracks ||
       sector > floppy_drive[drive_sel].info->sectors)
        return -1;

    floppy_motor_on(drive_sel);

    head &= 0x01;                       /* 00000001 */
    drive_sel &= 0x03;                  /* 00000011 */
    op_result = floppy_seek(drive_sel, head, track);
    if(op_result == -1){
        floppy_motor_off(drive_sel);
        return -1;
    }

    timeout = 0;
    while(timeout++ < FLOPPY_RETRY_MAX){
        floppy_send_byte(cmd);                                          /* send READ/WRITE command */
        floppy_send_byte((head << 2) | drive_sel);                      /* send head and drive number */
        floppy_send_byte(track);                                        /* send track number */
        floppy_send_byte(head);                                         /* send head number */
        floppy_send_byte(sector);                                       /* send sector number */
        floppy_send_byte(2);                                            /* send block size, 128 * 2 ^ N, so N = 2*/
        floppy_send_byte(floppy_drive[drive_sel].info->sectors);        /* send block number per track */
        floppy_send_byte(floppy_drive[drive_sel].info->gap_len);        /* send gap 3 length */
        floppy_send_byte(0xFF);                                         /* send data length */

        floppy_wait();

        result_st0 = floppy_get_byte();                                 /* get status 0 */
        result_st1 = floppy_get_byte();                                 /* get status 1 */
        result_st2 = floppy_get_byte();                                 /* get status 2 */
        result_track = floppy_get_byte();                               /* get track number */
        result_head = floppy_get_byte();                                /* get head number */
        result_sector = floppy_get_byte();                              /* get sector number */
        result_sector_size = floppy_get_byte();                         /* get sector size */

        rw_error = 0;

        /* ... */
        if(result_st0 & 0xC0 != 0){
            rw_error = 1;
        }
        /* drive not ready */
        if(result_st0 & 0x08 != 0){
            rw_error = 1;
        }
        /* end of track */
        if(result_st1 & 0x80 != 0){
            rw_error = 1;
        }
        /* CRC error */
        if(result_st1 & 0x20 != 0){
            rw_error = 1;
        }
        /* controller timeout */
        if(result_st1 & 0x10 != 0){
            rw_error = 1;
        }
        /* no data dound */
        if(result_st1 & 0x04 != 0){
            rw_error = 1;
        }
        /* not writable */
        if(result_st1 & 0x02 != 0){
            rw_error = 2;
        }
        /* no address mark found */
        if((result_st1 | result_st2) & 0x01 != 0){
            rw_error = 1;
        }
        /* deleted address mark */
        if(result_st2 & 0x40 != 0){
            rw_error = 1;
        }
        /* CRC error in data */
        if(result_st2 & 0x20 != 0){
            rw_error = 1;
        }
        /* wrong track */
        if(result_st2 & 0x10 != 0){
            rw_error = 1;
        }
        /* uPD765 sector not found */
        if(result_st2 & 0x04 != 0){
            rw_error = 1;
        }
        /* bad track */
        if(result_st2 & 0x02 != 0){
            rw_error = 1;
        }
        /* sector size not match */
        if(result_sector_size != 2){
            rw_error = 1;
        }

        /* read/write success */
        if(rw_error == 0){
            floppy_motor_off(drive_sel);
            return 0;
        }

        /* not writable */
        if(rw_error == 2){
            floppy_motor_off(drive_sel);
            return -2;
        }
    }

    floppy_motor_off(drive_sel);

    return -1;
}

/* read data from floppy disc */
INT8S floppy_read(INT8U drive_sel, INT32U logic_sector_num, INT16U sectors_to_read, void *p_buffer)
{
    INT8U *p_dma_buffer;
    INT8U head;
    INT8U track;
    INT8U sector;
    INT8S result;

    /* do some basic check */
    if(floppy_drive[drive_sel].info == NULL || floppy_drive[drive_sel].type == 0)
        return -1;
    if(sectors_to_read == 0)
        return -1;
    if(p_buffer == NULL)
        return -1;

    /* allocate low memory page */
    p_dma_buffer = (INT8U *)alloc_low_mem_page();
    if(p_dma_buffer == NULL)
        return -1;

    /* setup DMA to read data */
    dma_read(FLOPPY_DMA_CHANNEL, (INT32U)p_dma_buffer, 512);

    /* read floppy block */
    do{
        floppy_get_position(drive_sel, logic_sector_num, &head, &track, &sector);

        result = floppy_rw_block(drive_sel, logic_sector_num, FLOPPY_READ);

        if(result != 0)
            break;

        memcpy(p_buffer, p_dma_buffer, floppy_drive[drive_sel].info->sector_size);

        /* prepare to read next sector */
        logic_sector_num++;
        p_buffer += floppy_drive[drive_sel].info->sector_size;
        sectors_to_read--;

    }while(sectors_to_read > 0);

    free_mem_page(p_dma_buffer);

    return result;
}

/* write data to floppy disc */
INT8S floppy_write(INT8U drive_sel, INT8U logic_sector_num, INT16U sectors_to_write, void *p_buffer)
{
    INT8U *p_dma_buffer;
    INT8U head;
    INT8U track;
    INT8U sector;
    INT8S result;

    /* do some basic check */
    if(floppy_drive[drive_sel].info == NULL || floppy_drive[drive_sel].type == 0)
        return -1;
    if(sectors_to_write == 0)
        return -1;
    if(p_buffer == NULL)
        return -1;

    /* allocate low memory page */
    p_dma_buffer = (INT8U *)alloc_low_mem_page();
    if(p_dma_buffer == NULL)
        return -1;

    /* setup DMA to read data */
    dma_write(FLOPPY_DMA_CHANNEL, (INT32U)p_dma_buffer, floppy_drive[drive_sel].info->sector_size);

    /* read floppy block */
    do{
        memcpy(p_dma_buffer, p_buffer, floppy_drive[drive_sel].info->sector_size);

        floppy_get_position(drive_sel, logic_sector_num, &head, &track, &sector);

        result = floppy_rw_block(drive_sel, logic_sector_num, FLOPPY_WRITE);

        if(result != 0)
            break;

        /* prepare to read next sector */
        logic_sector_num++;
        p_buffer += floppy_drive[drive_sel].info->sector_size;
        sectors_to_write--;

    }while(sectors_to_write > 0);

    free_mem_page(p_dma_buffer);

    return result;
}

INT8S floppy_init()
{
    INT8U drives;

    /* get floppy drive info from CMOS, port number = 0x70, 0x71, register = 0x10 */
	outb(0x70, 0x10);
	drives = inb(0x71);

    floppy_drive[0].type = (drives >> 4);
    floppy_drive[0].info = &floppy_info[drives >> 4];

    floppy_drive[2].type = (drives & 0x0F);
    floppy_drive[2].info = &floppy_info[drives & 0x0F];

    floppy_interrupt_flag = FALSE;

    interrupt_set_isr_handler(FLOPPY_ISR_NUM, floppy_handler);

    floppy_reset();

    KERNEL_MSG(FALSE, TRUE, "floppy drives: master = %s, slave = %s\n",
               floppy_drive[0].info->type_name,
               floppy_drive[2].info->type_name);
}

