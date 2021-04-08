#include "cpu/instr.h"

make_instr_func(call_rel_v)
{
	cpu.esp -= data_size / 8;
	
	OPERAND mem, imm;
    mem.type = OPR_MEM;
	mem.data_size = data_size;
	mem.addr = cpu.esp;
	mem.val = eip + 1 + data_size / 8;
	mem.sreg = SREG_SS;
	
	imm.type = OPR_IMM;
	imm.data_size = data_size;
	imm.addr = eip + 1;
	imm.sreg = SREG_CS;
	
	operand_read(&imm);
	operand_write(&mem);
	
	OPERAND prt;
	prt.type = OPR_PRT;
	prt.val = eip + imm.val + 1 + data_size / 8;
	print_asm_1("call","", 1 + data_size / 8, &prt);
	
    cpu.eip += imm.val + 1 + data_size / 8;
    if (data_size == 16) cpu.eip &= 0xFFFF;
    
    return 0;
}
make_instr_func(call_rm_v)
{
	int len = 1;
	
	cpu.esp -= data_size / 8;
	
	OPERAND mem, rm;
	rm.data_size = data_size;
    mem.type = OPR_MEM;
	mem.data_size = data_size;
	mem.addr = cpu.esp;	
	mem.sreg = SREG_SS;
	len += modrm_rm(eip + 1, &rm);
	operand_read(&rm);
	mem.val = eip + len;	
	operand_write(&mem);
	
	print_asm_1("call","", len, &rm);
	
    cpu.eip = rm.val;
    if (data_size == 16) cpu.eip &= 0xFFFF;
    
    return 0;
}
