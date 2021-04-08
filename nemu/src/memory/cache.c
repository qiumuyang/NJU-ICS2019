#include "nemu.h"
#include "memory/cache.h"
#include "memory/memory.h"

#include <time.h>
#include <stdlib.h>

//#define CACHE_DEBUG
CacheLine cache[CACHE_GROUP][CACHE_LINE];

void init_cache()
{
	srand(time(0));
	for (int i = 0; i < CACHE_GROUP; i++)
		for (int j = 0; j < CACHE_LINE; j++)
			cache[i][j].valid = 0;	
}

uint32_t cache_read(paddr_t paddr, size_t len)
{
	uint32_t group = paddr / CACHE_LINE_SIZE % CACHE_GROUP;
	uint32_t caddr = paddr % CACHE_LINE_SIZE;
	uint32_t tag = paddr / CACHE_LINE_SIZE / CACHE_GROUP;
	
	uint32_t ret = 0;
	
	for (int i = 0; i < CACHE_LINE; i++)
	{
		if (cache[group][i].valid && cache[group][i].tag == tag)	// cache hit
		{
#ifdef CACHE_DEBUG
			printf("\n\e[0;34mcache output:\e[0m cache read \e[0;32mhit\e[0m\n");
#endif
			if (len + caddr > CACHE_LINE_SIZE)	// more than one cacheline
			{
				size_t len_m = len + caddr - CACHE_LINE_SIZE;
				len -= len_m;
				
				uint32_t c_ret = 0; ret = 0;
				memcpy(&c_ret, cache[group][i].data + caddr, len);
				memcpy(&ret, hw_mem + paddr + len, len_m);
				return (ret << (8 * len)) | c_ret;
			}
			else					// within one cacheline
			{
				ret = 0;
				memcpy(&ret, cache[group][i].data + caddr, len);
				return ret;
			}
		}
	}
	
	// cache miss
	int line = 0;
	
	while (line < CACHE_LINE) 
	{	
		if (!cache[group][line].valid) break;	// find vacant line
		line++;
	}
		
	if (line == CACHE_LINE) 					// cache group full
	{
		line = rand() % CACHE_LINE;				
	}
#ifdef CACHE_DEBUG
	printf("\n\e[0;34mcache output\e[0m: cache read \e[0;31mmiss\e[0m\n");
	printf("              paddr %x caddr %x len %d\n              group %d line %d tag %x\n", paddr, len, caddr, group, line, tag);
#endif
	memcpy(&cache[group][line].data, hw_mem + paddr - paddr % CACHE_LINE_SIZE, CACHE_LINE_SIZE);	// cache replace
	cache[group][line].valid = 1;
	cache[group][line].tag = tag;
	
	ret = 0;
	memcpy(&ret, hw_mem + paddr, len);
	return ret;
}

void cache_write(paddr_t paddr, size_t len, uint32_t data)
{
	memcpy(hw_mem + paddr, &data, len);			 // write through && not write allocate
	
	uint32_t group = paddr / CACHE_LINE_SIZE % CACHE_GROUP;
	uint32_t caddr = paddr % CACHE_LINE_SIZE;
	uint32_t tag = paddr / CACHE_LINE_SIZE / CACHE_GROUP;
#ifdef CACHE_DEBUG	
	bool miss = true;
#endif
	for (int i = 0; i < CACHE_LINE; i++)
	{
		if (cache[group][i].valid && cache[group][i].tag == tag)	// cache hit
		{
			
#ifdef CACHE_DEBUG
			miss = false;
			printf("\n\e[0;34mcache output:\e[0m cache write \e[0;32mhit\e[0m\n");
#endif
			if (len + caddr > CACHE_LINE_SIZE)	// more than one cacheline
			{
				size_t tlen = len + caddr - CACHE_LINE_SIZE;
				len = CACHE_LINE_SIZE - caddr;
				
				memcpy(cache[group][i].data + caddr, &data, len);
				
				paddr = paddr - paddr % CACHE_LINE_SIZE + CACHE_LINE_SIZE;
				cache_write(paddr, tlen, data >> (8 * len));
			}
			else					// within one cacheline
				memcpy(cache[group][i].data + caddr, &data, len);
		}
	}
#ifdef CACHE_DEBUG
	if (miss) printf("\n\e[0;34mcache output\e[0m: cache write \e[0;31mmiss\e[0m\n");
#endif
}
