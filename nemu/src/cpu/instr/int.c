#include "cpu/instr.h"
#include "cpu/intr.h"

make_instr_func(int_)
{
	OPERAND imm;
	imm.type = OPR_IMM;
	imm.data_size = 8;
	imm.sreg = SREG_CS;
	imm.addr = eip + 1;
	operand_read(&imm);
	
	print_asm_1("int","", 2, &imm);
	
	raise_sw_intr(imm.val);
	
	return 0;
}

static uint32_t pop()
{
    OPERAND mem;
    mem.type = OPR_MEM;
	mem.data_size = 32;
	mem.addr = cpu.esp;
	mem.sreg = SREG_SS;
    operand_read(&mem);
    cpu.esp += 4;
    return mem.val;
}

make_instr_func(iret)
{
	cpu.eip = pop();
	cpu.segReg[SREG_CS].val = pop() & 0xFFFF;
	load_sreg(SREG_CS);
	cpu.eflags.val = pop();
	//printf("eip: %08x cs: %x eflags: %x\n", cpu.eip, cpu.segReg[SREG_CS].val, cpu.eflags.val);
	print_asm_0("iret","", 1);
	
	return 0;
}
