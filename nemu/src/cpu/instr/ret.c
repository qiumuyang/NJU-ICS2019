#include "cpu/instr.h"

make_instr_func(ret_near)
{
	OPERAND mem;
    mem.type = OPR_MEM;
	mem.data_size = data_size;
	mem.addr = cpu.esp;
	mem.sreg = SREG_SS;
	
	operand_read(&mem);
    
    print_asm_0("ret","", 1);
    
    cpu.eip = mem.val;
    if (data_size == 16) cpu.eip &= 0xFFFF;
    
    cpu.esp += data_size / 8;
    
    return 0;
}

make_instr_func(ret_near_iw)
{
	OPERAND mem, imm;
	imm.type = OPR_IMM;
	imm.data_size = 16;
	imm.addr = eip + 1;
	imm.sreg = SREG_CS;
    mem.type = OPR_MEM;
	mem.data_size = data_size;
	mem.addr = cpu.esp;
	mem.sreg = SREG_SS;
	
	operand_read(&mem);
    operand_read(&imm);
     
    print_asm_0("ret","", 1);
    
    cpu.eip = mem.val;
    if (data_size == 16) cpu.eip &= 0xFFFF;
    
    cpu.esp += data_size / 8;
    cpu.esp += imm.val;
    
    return 0;
}
