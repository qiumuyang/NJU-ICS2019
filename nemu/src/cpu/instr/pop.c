#include "cpu/instr.h"

make_instr_func(pop_rax)
{
	instr_pop_reg(0)
}
make_instr_func(pop_rcx)
{
	instr_pop_reg(1)
}
make_instr_func(pop_rdx)
{
	instr_pop_reg(2)
}
make_instr_func(pop_rbx)
{
	instr_pop_reg(3)
}
make_instr_func(pop_rsp)
{
	instr_pop_reg(4)
}
make_instr_func(pop_rbp)
{
	instr_pop_reg(5)
}
make_instr_func(pop_rsi)
{
	instr_pop_reg(6)
}
make_instr_func(pop_rdi)
{
	instr_pop_reg(7)
}
make_instr_func(pop_mem_v)
{
	int len = 1;
	
    OPERAND rm, mem;
    rm.data_size = data_size;
    mem.type = OPR_MEM;
	mem.data_size = data_size;
	mem.addr = cpu.esp;
	mem.sreg = SREG_SS;
	
	len += modrm_rm(eip + 1, &rm);
    operand_read(&mem);
	operand_read(&rm);
    
    rm.val = mem.val;
    operand_write(&rm);
    
    if (data_size == 32) print_asm_1("pop","l", len, &rm);
    else print_asm_1("pop","w", len, &rm);
    
    cpu.esp += data_size / 8;
    
    return len;
}
make_instr_func(popa_v)
{	

	print_asm_0("popa","", 1);
	_PUSH_POP_A = true;
	pop_rdi(eip, opcode);
	pop_rsi(eip, opcode);
	pop_rbp(eip, opcode);
    
    //throw away esp
    cpu.esp += data_size / 8;
    //
    
    pop_rbx(eip, opcode);
    pop_rdx(eip, opcode);	
	pop_rcx(eip, opcode);
	pop_rax(eip, opcode);
	_PUSH_POP_A = false;
    return 1;	
}
