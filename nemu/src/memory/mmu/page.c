#include "cpu/cpu.h"
#include "memory/memory.h"

#ifdef IA32_PAGE
// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED
	uint32_t offset = laddr & 0xfff;
	uint32_t page = (laddr >> 12) & 0x3ff;
	uint32_t dir = (laddr >> 22) & 0x3ff;
	
	PDE pde;
	PTE pte;
	
	uint32_t pde_addr = (cpu.cr3.base << 12) + dir * 4;
	pde.val = hw_mem_read(pde_addr, 4);
	
	if (!pde.present)
	{
		//printf("PDE: addr%08x val%08x\n", pde_addr, pde.val);
		//printf("laddr: %08x\n", laddr);
		assert(pde.present == 1);
	}
	assert(pde.present == 1);
	
	
	uint32_t pte_addr = (pde.page_frame << 12) + page * 4;
	pte.val = hw_mem_read(pte_addr, 4);
	//assert(pte.present == 1);
	if (!pte.present)
	{
		//printf("PDE: addr%08x val%08x\n", pde_addr, pde.val);
		//printf("PTE: addr%08x val%08x\n", pte_addr, pte.val);
		//printf("laddr: %08x\n", laddr);
		assert(pte.present == 1);
	}
	
	return (pte.page_frame << 12) + offset;
	
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
#endif
