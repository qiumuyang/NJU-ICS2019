#ifndef __INSTR_INC_H__
#define __INSTR_INC_H__

#define instr_inc_reg(x) \
    int tmp = cpu.eflags.CF; \
    if (data_size == 16) \
        cpu.gpr[x]._16 = alu_add(1, cpu.gpr[x]._16, data_size); \
    else \
        cpu.gpr[x]._32 = alu_add(1, cpu.gpr[x]._32, data_size); \
    cpu.eflags.CF = tmp; \
    OPERAND reg; \
	reg.type = OPR_REG; \
	reg.data_size = data_size; \
	reg.addr = opcode & 7; \
	print_asm_1("inc","", 1, &reg); \
    return 1;

make_instr_func(inc_rax);
make_instr_func(inc_rcx);
make_instr_func(inc_rdx);
make_instr_func(inc_rbx);
make_instr_func(inc_rsp);
make_instr_func(inc_rbp);
make_instr_func(inc_rsi);
make_instr_func(inc_rdi);

make_instr_func(inc_rm_b);
make_instr_func(inc_rm_v);

#endif
