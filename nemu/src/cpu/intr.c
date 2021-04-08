#include "cpu/intr.h"
#include "cpu/instr.h"
#include "memory/memory.h"

static void push(uint32_t val, int sz)
{
	OPERAND mem;
	mem.data_size = sz;
	mem.type = OPR_MEM;
	mem.sreg = SREG_SS;
    cpu.esp -= sz / 8;
    mem.addr = cpu.esp;
    mem.val = val;
    
    operand_write(&mem);
}

void raise_intr(uint8_t intr_no)
{
#ifdef IA32_INTR
	/*printf("Please implement raise_intr()");
	assert(0);*/
	
	push(cpu.eflags.val, 32);
	push(cpu.segReg[SREG_CS].val, 32);	// CS in TrapFrame treated as 32-bit
	push(cpu.eip, 32);
	
	GateDesc gateDesc;
	uint32_t gateDescAddr = cpu.idtr.base + intr_no * 8;
	gateDesc.val[0] = laddr_read(gateDescAddr, 4);
	gateDesc.val[1] = laddr_read(gateDescAddr + 4, 4);
	assert(gateDesc.present == 1);
	
	if (gateDesc.type == 0xe) cpu.eflags.IF = 0;
	
	uint32_t offset = (gateDesc.offset_31_16 << 16) | gateDesc.offset_15_0;
	cpu.segReg[SREG_CS].val = gateDesc.selector;
	load_sreg(SREG_CS);
	
	cpu.eip = segment_translate(offset, SREG_CS);
	
#endif
}

void raise_sw_intr(uint8_t intr_no)
{
	// return address is the next instruction
	cpu.eip += 2;
	raise_intr(intr_no);
}
