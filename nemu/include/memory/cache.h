#ifndef __CACHE_H__
#define __CACHE_H__

#include "nemu.h"

// 1K * 64B cache
#define CACHE_SIZE_B 1024
#define CACHE_GROUP 128
#define CACHE_LINE 8
#define CACHE_LINE_SIZE 64
// CACHE_LINE can be changed to get N-way set associative
// but this will change cache capacity

typedef struct
{
    uint16_t valid : 1;
    uint16_t tag : 15;
	uint8_t  data[CACHE_LINE_SIZE];
} CacheLine;

extern CacheLine cache[][CACHE_LINE];

void init_cache();

uint32_t cache_read(paddr_t paddr, size_t len);
void cache_write(paddr_t paddr, size_t len, uint32_t data);

#endif
