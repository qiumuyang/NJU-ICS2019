#include "cpu/instr.h"

make_instr_func(leave)
{
	if (data_size == 16)
	{
		cpu.gpr[4]._16 = cpu.gpr[5]._16;
	}
	else
	{
		cpu.gpr[4]._32 = cpu.gpr[5]._32;
	}
	print_asm_0("leave","", 1);
	OPERAND mem;
    mem.type = OPR_MEM;
	mem.data_size = data_size;
	mem.addr = cpu.esp;
	mem.sreg = SREG_SS;
    operand_read(&mem);
    cpu.esp += data_size / 8;
    if (data_size == 16)
        cpu.gpr[5]._16 = mem.val;
    else cpu.gpr[5]._32 = mem.val;
    return 1;
}
