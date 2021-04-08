#include "common.h"
#include "memory.h"
#include <string.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)
#define NR_PT ((SCR_SIZE + PT_SIZE - 1) / PT_SIZE) // number of page tables to cover the vmem

PDE *get_updir();

void create_video_mapping()
{

	/* TODO: create an identical mapping from virtual memory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memeory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */
	uint32_t addr = VMEM_ADDR;
	
	PDE *pdir = get_updir();
	
	PTE *ptable = (void *)(pdir[0].val & ~0xfff);
	
	pdir[0].val |= 0x7;	// make it present
	
	while (addr < VMEM_ADDR + SCR_SIZE)
	{
		uint32_t idx = (addr >> 12) & 0x3ff;
		ptable[idx].val = (addr & ~0xfff)| 0x7;
		addr += 0x400;	// page_size
	}
	//panic("please implement me");
}

void video_mapping_write_test()
{
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for (i = 0; i < SCR_SIZE / 4; i++)
	{
		buf[i] = i;
	}
}

void video_mapping_read_test()
{
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for (i = 0; i < SCR_SIZE / 4; i++)
	{
		assert(buf[i] == i);
	}
}

void video_mapping_clear()
{
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}
