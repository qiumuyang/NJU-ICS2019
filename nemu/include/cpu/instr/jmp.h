#ifndef __INSTR_JMP_H__
#define __INSTR_JMP_H__

make_instr_func(jmp_near);
make_instr_func(jmp_rel_v);
make_instr_func(jmp_rel_b);

make_instr_func(jmp_rm_v);
make_instr_func(jmp_mem_v);     //??????
make_instr_func(jmp_far_imm);
#endif
