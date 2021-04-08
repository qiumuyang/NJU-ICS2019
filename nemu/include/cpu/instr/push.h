#ifndef __INSTR_PUSH_H__
#define __INSTR_PUSH_H__

bool _PUSH_POP_A;

#define instr_push_reg(x) \
    OPERAND mem; \
    if (data_size == 16) \
        mem.val = cpu.gpr[x]._16; \
    else mem.val = cpu.gpr[x]._32; \
    cpu.esp -= data_size / 8; \
    mem.type = OPR_MEM; \
	mem.data_size = data_size; \
	mem.addr = cpu.esp; \
	mem.sreg = SREG_SS; \
    operand_write(&mem); \
    OPERAND reg; \
	reg.type = OPR_REG; \
	reg.data_size = data_size; \
	reg.addr = opcode & 7; \
	if (!_PUSH_POP_A) print_asm_1("push","", 1, &reg); \
    return 1;

make_instr_func(push_rax);
make_instr_func(push_rcx);
make_instr_func(push_rdx);
make_instr_func(push_rbx);
make_instr_func(push_rsp);
make_instr_func(push_rbp);
make_instr_func(push_rsi);
make_instr_func(push_rdi);
make_instr_func(push_mem_v);
make_instr_func(push_imm_b);
make_instr_func(push_imm_v);
make_instr_func(pusha_v);

/*
make_instr_func(push_es);
make_instr_func(push_cs);
make_instr_func(push_ss);
make_instr_func(push_ds);
make_instr_func(push_fs);
make_instr_func(push_gs);
*/
#endif
