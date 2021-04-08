#include "cpu/instr.h"
#include "cpu/alu.h"
make_instr_func(neg_rm_v)
{
	int len = 1;
	OPERAND rm;
	rm.data_size = data_size;
	len += modrm_rm(eip + 1, &rm);
	operand_read(&rm);
	
	rm.val *= -1;
	
	set_PF(rm.val);
	set_ZF(rm.val, data_size);
	set_SF(rm.val, data_size);
	cpu.eflags.CF = (rm.val != 0);
	cpu.eflags.OF = (rm.val == (1 << (data_size - 1)));
	
	if (data_size == 16) print_asm_1("neg","w", len, &rm);
	else print_asm_1("neg","l", len, &rm);
	
	operand_write(&rm);
	
	return len;
}

make_instr_func(neg_rm_b)
{
	int len = 1;
	OPERAND rm;
	rm.data_size = 8;
	len += modrm_rm(eip + 1, &rm);
	operand_read(&rm);
	
	rm.val *= -1;
	
	set_PF(rm.val);
	set_ZF(rm.val, 8);
	set_SF(rm.val, 8);
	cpu.eflags.CF = (rm.val != 0);
	cpu.eflags.OF = (rm.val == 0x80);
	
	print_asm_1("neg","b", len, &rm);
	
	operand_write(&rm);
	
	return len;
}
