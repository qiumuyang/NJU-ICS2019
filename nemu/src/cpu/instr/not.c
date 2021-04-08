#include "cpu/instr.h"

make_instr_func(not_rm_b)
{
    int len = 1;
    
    OPERAND rm;
	rm.data_size = 8;
	
	len += modrm_rm(eip + 1, &rm);
    operand_read(&rm);
    rm.val = (uint8_t)(~rm.val);
    operand_write(&rm);
    
    print_asm_1("not","", len, &rm);
    
    return len;
}
make_instr_func(not_rm_v)
{
    int len = 1;
    
    OPERAND rm;
	rm.data_size = data_size;
	
	len += modrm_rm(eip + 1, &rm);
    operand_read(&rm);
    rm.val = (data_size == 16) ? (uint16_t)(~rm.val) : (uint32_t)(~rm.val);
    operand_write(&rm);
    
    print_asm_1("not","", len, &rm);
    
    return len;
}
