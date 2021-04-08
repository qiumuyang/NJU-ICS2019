#ifndef __INSTR_DEC_H__
#define __INSTR_DEC_H__

#define instr_dec_reg(x) \
    int tmp = cpu.eflags.CF; \
    if (data_size == 16) \
        cpu.gpr[x]._16 = alu_sub(1, cpu.gpr[x]._16, data_size); \
    else \
        cpu.gpr[x]._32 = alu_sub(1, cpu.gpr[x]._32, data_size); \
    cpu.eflags.CF = tmp; \
    OPERAND reg; \
	reg.type = OPR_REG; \
	reg.data_size = data_size; \
	reg.addr = opcode & 7; \
	print_asm_1("dec","", 1, &reg); \
    return 1;

make_instr_func(dec_rax);
make_instr_func(dec_rcx);
make_instr_func(dec_rdx);
make_instr_func(dec_rbx);
make_instr_func(dec_rsp);
make_instr_func(dec_rbp);
make_instr_func(dec_rsi);
make_instr_func(dec_rdi);

make_instr_func(dec_rm_b);
make_instr_func(dec_rm_v);

#endif
