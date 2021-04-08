#ifndef __INSTR_POP_H__
#define __INSTR_POP_H__

#define instr_pop_reg(x) \
    OPERAND mem; \
    mem.type = OPR_MEM; \
	mem.data_size = data_size; \
	mem.addr = cpu.esp; \
	mem.sreg = SREG_SS; \
    operand_read(&mem); \
    cpu.esp += data_size / 8; \
    if (data_size == 16) \
        cpu.gpr[x]._16 = mem.val; \
    else cpu.gpr[x]._32 = mem.val; \
    OPERAND reg; \
	reg.type = OPR_REG; \
	reg.data_size = data_size; \
	reg.addr = opcode & 7; \
	if (!_PUSH_POP_A) print_asm_1("pop","", 1, &reg); \
    return 1;

make_instr_func(pop_rax);
make_instr_func(pop_rcx);
make_instr_func(pop_rdx);
make_instr_func(pop_rbx);
make_instr_func(pop_rsp);
make_instr_func(pop_rbp);
make_instr_func(pop_rsi);
make_instr_func(pop_rdi);
make_instr_func(pop_mem_v);
make_instr_func(popa_v);
//make_instr_func(pop_imm_b);
//make_instr_func(pop_imm_v);
/*
make_instr_func(pop_es);
make_instr_func(pop_cs);
make_instr_func(pop_ss);
make_instr_func(pop_ds);
make_instr_func(pop_fs);
make_instr_func(pop_gs);
*/
#endif
