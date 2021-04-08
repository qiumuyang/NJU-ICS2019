#include "cpu/instr.h"
#ifdef IA32_PAGE
make_instr_func(lgdt)
{
	int len = 1;
	
	OPERAND rm;
	rm.data_size = data_size;
	len += modrm_rm(eip + 1, &rm);
	operand_read(&rm);
	
	uint32_t addr = rm.addr;
	cpu.gdtr.limit = laddr_read(addr, 2);
	cpu.gdtr.base = laddr_read(addr + 2, 4);
	
	print_asm_1("lgdt","", len, &rm);
	return len;
}
#else
make_instr_func(lgdt)
{
	int len = 1;
	
	OPERAND rm;
	rm.data_size = data_size;
	len += modrm_rm(eip + 1, &rm);
	operand_read(&rm);
	
	//uint32_t addr = rm.addr;
	//cpu.gdtr.limit = hw_mem_read(addr, 2);
	//cpu.gdtr.base = hw_mem_read(addr + 2, 4);
	
	print_asm_1("lgdt","", len, &rm);
	return len;
}
#endif
