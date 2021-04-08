#include "cpu/instr.h"

make_instr_func(push_rax)
{
	instr_push_reg(0)
}
make_instr_func(push_rcx)
{
	instr_push_reg(1)
}
make_instr_func(push_rdx)
{
	instr_push_reg(2)
}
make_instr_func(push_rbx)
{
	instr_push_reg(3)
}
make_instr_func(push_rsp)
{
	instr_push_reg(4)
}
make_instr_func(push_rbp)
{
	instr_push_reg(5)
}
make_instr_func(push_rsi)
{
	instr_push_reg(6)
}
make_instr_func(push_rdi)
{
	instr_push_reg(7)
}
make_instr_func(push_mem_v)
{
	int len = 1;
	
    OPERAND rm;
    rm.data_size = data_size;
    
	len += modrm_rm(eip + 1, &rm);
    
    operand_read(&rm);
    
    cpu.esp -= data_size / 8;
    rm.addr = cpu.esp;
    operand_write(&rm);
    
    if (data_size == 32) print_asm_1("push","l", len, &rm);
    else print_asm_1("push","w", len, &rm);
    
    return len;
}
make_instr_func(push_imm_b)
{
	OPERAND mem, imm;
	mem.data_size = data_size;
	mem.type = OPR_MEM;
	mem.sreg = SREG_SS;
    imm.type = OPR_IMM;
	imm.data_size = 8;
	imm.addr = eip + 1;
    imm.sreg = SREG_CS;
    
    operand_read(&imm);
    
    cpu.esp -= data_size / 8;
    mem.addr = cpu.esp;
    
    mem.val = sign_ext(imm.val, 8);
    operand_write(&mem);
    
   	print_asm_1("push","b", 2, &imm);
    
    return 2;
}
make_instr_func(push_imm_v)
{
	OPERAND mem, imm;
	mem.data_size = data_size;
	mem.type = OPR_MEM;
	mem.sreg = SREG_SS;
    imm.type = OPR_IMM;
	imm.data_size = data_size;
	imm.sreg = SREG_CS;
	imm.addr = eip + 1;
    
    operand_read(&imm);
    
    cpu.esp -= data_size / 8;
    mem.addr = cpu.esp;
    mem.val = imm.val;
    
    operand_write(&mem);
    
    if (data_size == 32) print_asm_1("push","l", 1 + data_size / 8, &imm);
    else print_asm_1("push","w", 1 + data_size / 8, &imm);
    
    return 1 + data_size / 8;	
}
make_instr_func(pusha_v)
{	
	OPERAND mem;
	mem.data_size = data_size;
	mem.type = OPR_MEM;
	mem.val = (data_size == 16) ? cpu.gpr[4]._16 : cpu.gpr[4]._32;
	mem.sreg = SREG_SS;
	
	print_asm_0("pusha","", 1);
	_PUSH_POP_A = true;
	push_rax(eip, opcode);
	push_rcx(eip, opcode);
	push_rdx(eip, opcode);
	push_rbx(eip, opcode);
	
	//push esp
	cpu.esp -= data_size / 8;
    mem.addr = cpu.esp;
    operand_write(&mem);
    
    push_rbp(eip, opcode);
    push_rsi(eip, opcode);
    push_rdi(eip, opcode);
    _PUSH_POP_A = false;
    //cpu.esp += data_size / 8;
    
    return 1;	
}
/*
make_instr_func(push_es)
{
	cpu.esp -= data_size / 8;
    OPERAND mem;
    mem.type = OPR_MEM;
	mem.data_size = data_size;
	mem.addr = cpu.esp;
   	mem.val = cpu.segReg[0];
    operand_write(&mem);
    return 1;
}
make_instr_func(push_cs)
{
	cpu.esp -= data_size / 8;
    OPERAND mem;
    mem.type = OPR_MEM;
	mem.data_size = data_size;
	mem.addr = cpu.esp;
    mem.val = cpu.segReg[1];
    operand_write(&mem);
    return 1;
}
make_instr_func(push_ss)
{
	cpu.esp -= data_size / 8;
    OPERAND mem;
    mem.type = OPR_MEM;
	mem.data_size = data_size;
	mem.addr = cpu.esp;
    mem.val = cpu.segReg[2];
    operand_write(&mem);
    return 1;
}
make_instr_func(push_ds)
{
	cpu.esp -= data_size / 8;
    OPERAND mem;
    mem.type = OPR_MEM;
	mem.data_size = data_size;
	mem.addr = cpu.esp;
    mem.val = cpu.segReg[3];
    operand_write(&mem);
    return 1;
}
make_instr_func(push_fs)
{
	cpu.esp -= data_size / 8;
    OPERAND mem;
    mem.type = OPR_MEM;
	mem.data_size = data_size;
	mem.addr = cpu.esp;
    mem.val = cpu.segReg[4];
    operand_write(&mem);
    return 1;
}
make_instr_func(push_gs)
{
	cpu.esp -= data_size / 8;
    OPERAND mem;
    mem.type = OPR_MEM;
	mem.data_size = data_size;
	mem.addr = cpu.esp;
    mem.val = cpu.segReg[5];
    operand_write(&mem);
    return 1;
}*/
