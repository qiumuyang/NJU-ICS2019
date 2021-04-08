#include "cpu/instr.h"
#include "device/port_io.h"

make_instr_func(in_b)
{
	cpu.gpr[REG_AL]._8[0] = pio_read(cpu.gpr[REG_DX]._16, 1);
	return 1;
}

make_instr_func(in_v)
{
	uint32_t data = pio_read(cpu.gpr[REG_DX]._16, data_size / 8);
	if (data_size == 32)
		cpu.eax = data;
	else
		cpu.gpr[REG_AX]._16 = data & 0xFFFF;
	return 1;
}
