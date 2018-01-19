/*
 *****************************************************************************
 * mm.h (Memory Management)
 * shiuan 20101226
 *****************************************************************************
 */

#ifndef _MM_H_
#define _MM_H_

#include "../types.h"
#include "physical.h"
#include "segment.h"
#include "paging.h"
#include "kmalloc.h"
#include "vmalloc.h"

void mm_init();

#endif
