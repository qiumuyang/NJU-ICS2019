#include "cpu/instr.h"

make_instr_func(jmp_near)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = data_size;
        rel.addr = eip + 1;

        operand_read(&rel);

        int offset = sign_ext(rel.val, data_size);
        // thank Ting Xu from CS'17 for finding this bug
        print_asm_1("jmp", "", 1 + data_size / 8, &rel);

        cpu.eip += offset;

        return 1 + data_size / 8;
}

make_instr_func(jmp_rel_v)
{
	OPERAND imm;
	imm.type = OPR_IMM;
	imm.data_size = data_size;
	imm.addr = eip + 1;
	imm.sreg = SREG_CS;
	operand_read(&imm);
	
	OPERAND prt;
	prt.type = OPR_PRT;
	prt.val = eip + imm.val + data_size / 8 + 1;
	print_asm_1("jmp","", data_size / 8 + 1, &prt);
	
	cpu.eip += imm.val;
	if (data_size == 16) cpu.eip &= 0xFFFF;
	
	return data_size / 8 + 1;
}
make_instr_func(jmp_rel_b)
{
	OPERAND imm;
	imm.type = OPR_IMM;
	imm.data_size = 8;
	imm.addr = eip + 1;
	imm.sreg = SREG_CS;
	operand_read(&imm);
	
	OPERAND prt;
	prt.type = OPR_PRT;
	prt.val = eip + sign_ext(imm.val, 8) + 2;
	print_asm_1("jmp","", 2, &prt);
	
	cpu.eip += sign_ext(imm.val, 8);
	return 2;
}

make_instr_func(jmp_rm_v)
{
	int len = 1;
	OPERAND rm;
	rm.data_size = data_size;
	
	len += modrm_rm(eip + 1, &rm);
	operand_read(&rm);
	
	print_asm_1("jmp","", len, &rm);
	
	cpu.eip = rm.val;
	if (data_size == 16) cpu.eip &= 0xFFFF;
	
	return 0;
}
make_instr_func(jmp_mem_v)
{
	int len = 1;
	
	OPERAND rm, mem;
	rm.data_size = data_size;
	mem.data_size = data_size;
	mem.type = OPR_MEM;
	mem.type = SREG_DS;
	
	len += modrm_rm(eip + 1, &rm);
	operand_read(&rm);
	mem.addr = rm.val;
	operand_read(&mem);
	
	print_asm_0("jmp_mem","_ToDo", len);
	
	cpu.eip = mem.val;
	
	if (data_size == 16) cpu.eip &= 0xFFFF;
	
	return 0;
}

make_instr_func(jmp_far_imm)
{
	int len = 1 + 2 + data_size / 8;
	
	OPERAND cs, imm;
	cs.type = OPR_IMM;
	cs.data_size = 16;
	cs.addr = eip + data_size / 8 + 1;
	operand_read(&cs);
	imm.type = OPR_IMM;
	imm.data_size = data_size;
	imm.addr = eip + 1;
	operand_read(&imm);
	
	print_asm_1("jmp","", len, &imm);
	
	cpu.segReg[SREG_CS].val = cs.val;
	cpu.eip = imm.val;
	
	load_sreg(SREG_CS);
	
	if (data_size == 16) cpu.eip &= 0xFFFF;
	
	return 0;
}
