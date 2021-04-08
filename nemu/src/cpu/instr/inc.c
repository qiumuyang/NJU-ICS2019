#include "cpu/instr.h"

make_instr_func(inc_rax)
{
	instr_inc_reg(0)
}
make_instr_func(inc_rcx)
{
	instr_inc_reg(1)
}
make_instr_func(inc_rdx)
{
	instr_inc_reg(2)
}
make_instr_func(inc_rbx)
{
	instr_inc_reg(3)
}
make_instr_func(inc_rsp)
{
	instr_inc_reg(4)
}
make_instr_func(inc_rbp)
{
	instr_inc_reg(5)
}
make_instr_func(inc_rsi)
{
	instr_inc_reg(6)
}
make_instr_func(inc_rdi)
{
	instr_inc_reg(7)
}
make_instr_func(inc_rm_b)
{
	int len = 1;
	
	OPERAND rm;
	rm.data_size = 8;
	
	len += modrm_rm(eip + 1, &rm); 
	operand_read(&rm);
	
	int tmp = cpu.eflags.CF;
	rm.val = alu_add(1, rm.val, 8);
	cpu.eflags.CF = tmp;
	
	print_asm_1("inc","b",len,&rm);
	operand_write(&rm);
	
	return len;
}
make_instr_func(inc_rm_v)
{
	int len = 1;
	
	OPERAND rm;
	rm.data_size = data_size;
	
	len += modrm_rm(eip + 1, &rm); 
	operand_read(&rm);
	
	int tmp = cpu.eflags.CF;
	rm.val = alu_add(1, rm.val, data_size);
	cpu.eflags.CF = tmp;
	
	print_asm_1("inc","",len,&rm);
	operand_write(&rm);
	
	return len;
}
