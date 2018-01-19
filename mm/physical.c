/*
 *****************************************************************************
 * physical.c (Physical Memory Management)
 * shiuan 20110102
 *****************************************************************************
 */

#include "physical.h"
#include "paging.h"
#include "../mb_info.h"

static INT8U *p_mem_bitmap;
static INT32U physical_mem_size;
static INT32U available_mem_start;
static INT32U free_mem_size;

/* get full physical memory size */
INT32U get_physical_mem_size()
{
    return physical_mem_size;
}

/* get free memory size */
INT32U get_free_mem_size()
{
    return free_mem_size;
}

/* allocate memory page, each page size equal 4K */
void *alloc_physical_mem_page(INT32U mem_limit_start, INT32U mem_limit_end)
{
    INT8U *p_bitmap;
    INT8U bits;
    INT32U mem_addr;

    p_bitmap = p_mem_bitmap;
    mem_addr = mem_limit_start;

    if(free_mem_size == 0)
        return NULL;
    if(mem_limit_end > physical_mem_size)
        mem_limit_end = physical_mem_size;
    if(mem_limit_start > mem_limit_end - PAGE_MEM_SIZE)
        return NULL;
    if(mem_limit_start < available_mem_start)
        return NULL;

    mem_addr -= mem_addr % (PAGE_MEM_SIZE * 8);
    p_bitmap += (mem_limit_start / PAGE_MEM_SIZE / 8);

    /* scan free page */
    while(mem_addr < mem_limit_end){
        bits = *p_bitmap;

        if(bits != 0){
            while(bits != 0){
                if((bits & 0x80) != 0){
                    *p_bitmap &= ~(0x80 >> (mem_addr / PAGE_MEM_SIZE % 8));
                    free_mem_size -= PAGE_MEM_SIZE;
                    return (void *)mem_addr;
                }

                bits <<= 1;
                mem_addr += PAGE_MEM_SIZE;
            }
        }
        else{
            p_bitmap++;
            mem_addr += PAGE_MEM_SIZE * 8;
        }
    }

    return NULL;
}

/* allocate low memory page */
void *alloc_low_mem_page()
{
    return alloc_physical_mem_page(available_mem_start, PHYSICAL_LOW_MEM);
}

/* allocate high memory page */
void *alloc_high_mem_page()
{
    return alloc_physical_mem_page(PHYSICAL_LOW_MEM, physical_mem_size);
}

void *alloc_mem_page()
{
    void *mem_addr;

    mem_addr = alloc_high_mem_page();

    if(mem_addr == NULL)
        mem_addr = alloc_low_mem_page();

    return mem_addr;
}

void free_mem_page(void *mem_addr)
{
    INT32U bytes;
    INT8U bits;
    INT8U *p_bitmap;

    if((INT32U)mem_addr < available_mem_start)
        return;

    p_bitmap = p_mem_bitmap;

    bytes = (INT32U)mem_addr / PAGE_MEM_SIZE / 8;
    bits = (INT32U)mem_addr / PAGE_MEM_SIZE % 8;

    *(p_bitmap + bytes) |= (0x80 >> bits);

    free_mem_size += PAGE_MEM_SIZE;
}

/* inital physical memory bitmap
   (calculate there are how many free memory for kernel and user) */
void physical_mem_init(INT32U free_mem_start)
{
    extern MULTIBOOT_INFO *_p_boot_info;
    INT32U mem_bitmap_size;
    INT8U *p_bitmap;
    ADDR_RANGE_DESC *p_mem_desc;

    /* calculate full physical memory size */
    p_mem_desc = (ADDR_RANGE_DESC *)_p_boot_info->mmap_addr;
    physical_mem_size = 0;
    while((INT32U)p_mem_desc < _p_boot_info->mmap_addr + _p_boot_info->mmap_length){
        physical_mem_size += p_mem_desc->Length;
        p_mem_desc++;
    }

    /* calculate bitmap size */
    mem_bitmap_size = (physical_mem_size / PAGE_MEM_SIZE / 8) + 1;
    if(mem_bitmap_size > physical_mem_size - free_mem_start)
        kernel_panic("cannot create memory bitmap");

    p_mem_bitmap = (INT8U *)free_mem_start;
    p_bitmap = p_mem_bitmap;
    memset((void *)p_bitmap, 0x00, mem_bitmap_size);

    /* get free memory start address(must align 4K) */
    free_mem_start += mem_bitmap_size;
    if(free_mem_start % PAGE_MEM_SIZE != 0){
        free_mem_start -= free_mem_start % PAGE_MEM_SIZE;
        free_mem_start += PAGE_MEM_SIZE;
    }

    available_mem_start = free_mem_start;

    /* setup physical memory bitmap and calculate free memory size */
    free_mem_size = 0;
    p_bitmap += (free_mem_start / PAGE_MEM_SIZE / 8);
    while(free_mem_start < physical_mem_size){
        *p_bitmap |= (0x80 >> (free_mem_start / PAGE_MEM_SIZE % 8));
        free_mem_size += PAGE_MEM_SIZE;
        free_mem_start += PAGE_MEM_SIZE;

        if(free_mem_start / PAGE_MEM_SIZE % 8 == 0)
            p_bitmap++;
    }
}

