#include "cpu/instr.h"

make_instr_func(dec_rax)
{
	instr_dec_reg(0)
}
make_instr_func(dec_rcx)
{
	instr_dec_reg(1)
}
make_instr_func(dec_rdx)
{
	instr_dec_reg(2)
}
make_instr_func(dec_rbx)
{
	instr_dec_reg(3)
}
make_instr_func(dec_rsp)
{
	instr_dec_reg(4)
}
make_instr_func(dec_rbp)
{
	instr_dec_reg(5)
}
make_instr_func(dec_rsi)
{
	instr_dec_reg(6)
}
make_instr_func(dec_rdi)
{
	instr_dec_reg(7)
}
make_instr_func(dec_rm_b)
{
	int len = 1;
	
	OPERAND rm;
	rm.data_size = 8;
	
	len += modrm_rm(eip + 1, &rm); 
	operand_read(&rm);
	
	int tmp = cpu.eflags.CF;
	rm.val = alu_sub(1, rm.val, 8);
	cpu.eflags.CF = tmp;
	
	print_asm_1("dec","b",len,&rm);
	operand_write(&rm);
	
	return len;
}
make_instr_func(dec_rm_v)
{
	int len = 1;
	
	OPERAND rm;
	rm.data_size = data_size;
	
	len += modrm_rm(eip + 1, &rm); 
	operand_read(&rm);
	
	
	int tmp = cpu.eflags.CF;
	rm.val = alu_sub(1, rm.val, data_size);
	cpu.eflags.CF = tmp;
	
	print_asm_1("dec","",len,&rm);
	
	operand_write(&rm);
	
	return len;
}
