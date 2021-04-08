#include "cpu/instr.h"
#ifdef IA32_INTR
make_instr_func(lidt)
{
	int len = 1;
	
	OPERAND rm;
	rm.data_size = data_size;
	len += modrm_rm(eip + 1, &rm);
	operand_read(&rm);
	
	uint32_t addr = rm.addr;
	cpu.idtr.limit = laddr_read(addr, 2);
	cpu.idtr.base = laddr_read(addr + 2, 4);
	
	print_asm_1("lidt","", len, &rm);
	return len;
}
#else
make_instr_func(lidt)
{
	int len = 1;
	
	return len;
}
#endif
