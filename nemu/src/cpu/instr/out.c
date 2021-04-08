#include "cpu/instr.h"
#include "device/port_io.h"

make_instr_func(out_b)
{
	pio_write(cpu.gpr[REG_DX]._16, 1, cpu.gpr[REG_AL]._8[0]);
	return 1;
}

make_instr_func(out_v)
{
	uint32_t data = (data_size == 32 ? cpu.eax : cpu.gpr[REG_AX]._16);
	pio_write(cpu.gpr[REG_DX]._16, data_size / 8, data);
	return 1;
}
