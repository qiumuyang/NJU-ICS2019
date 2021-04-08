#include "cpu/cpu.h"

void set_CF_add(uint32_t result, uint32_t src, size_t data_size) 
{ 
	result = sign_ext(result & (0xFFFFFFFF >> (32 - data_size)), data_size); 
	src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size); 
	cpu.eflags.CF = result < src; 
}
void set_CF_sub(uint32_t dest, uint32_t src, size_t data_size) 
{ 
	dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size); 
	src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size); 
	cpu.eflags.CF = dest < src;
}
void set_CF_sal(uint32_t dest, uint32_t src, size_t data_size) 
{ 
	cpu.eflags.CF = ((dest << (src - 1)) & (0x1 << (data_size - 1))) >> (data_size - 1);
}
void set_CF_sar(uint32_t dest, uint32_t src, size_t data_size) 
{ 
	cpu.eflags.CF = (dest >> (src - 1)) & 1;
}
void set_CF_OF_mul(uint64_t result, size_t data_size) 
{ 
	switch(data_size)
	{ 
		case 8: result = result & 0xFF00;
				result = result >> data_size;
				break; 
		case 16:result = result & 0xFFFF0000;
				result = result >> data_size;
				break; 
		case 32:result = result & 0xFFFFFFFF00000000;
				result = result >> data_size;
				break; 
		default:break;
	} 
	cpu.eflags.CF = (result != 0);
	cpu.eflags.OF = (result != 0);
}
void set_CF_OF_imul(int64_t result, size_t data_size) 
{ 
	switch(data_size)
	{ 
		case 8: result = result & 0xFF00;
				result = result >> data_size;
				break; 
		case 16:result = result & 0xFFFF0000; 
				result = result >> data_size;
				break; 
		case 32:result = result & 0xFFFFFFFF00000000;
				result = result >> data_size;
				break; 
		default:break;
	} 
	cpu.eflags.CF = (result != 0);
	cpu.eflags.OF = (result != 0);
}
void set_ZF(uint32_t result, size_t data_size) 
{ 
	result = result & (0xFFFFFFFF >> (32 - data_size)); 
	cpu.eflags.ZF = (result == 0); 
}
void set_SF(uint32_t result, size_t data_size) 
{ 
	result = sign_ext(result & (0xFFFFFFFF >> (32 - data_size)), data_size); 
	cpu.eflags.SF = sign(result); 
}
void set_PF(uint32_t result)
{
	result = result & 0xFF;
	uint32_t ret = 1;
	while (result)
	{
		ret = (result & 1) == 1 ? (!ret) : (ret);
		result >>= 1;
	}
	cpu.eflags.PF = ret;
}
void set_OF_add(uint32_t result, uint32_t src, uint32_t dest, size_t data_size)
{ 
	switch(data_size)
	{ 
		case 8: result = sign_ext(result & 0xFF, 8);
			src = sign_ext(src & 0xFF, 8);
			dest = sign_ext(dest & 0xFF, 8);
			break; 
		case 16: result = sign_ext(result & 0xFFFF, 16); 
			src = sign_ext(src & 0xFFFF, 16); 
			dest = sign_ext(dest & 0xFFFF, 16); 
			break; 
		default: break;// do nothing
	} 
	if (sign(src) == sign(dest)) 
	{ 
		if (sign(src) != sign(result)) cpu.eflags.OF = 1; 
		else cpu.eflags.OF = 0; 
	} 
	else 
	{ 
		cpu.eflags.OF = 0; 
	}
}
uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
	uint32_t res = 0; 
	res = dest + src;

	set_CF_add(res, src, data_size); 
	set_PF(res); 
	// set_AF();  我们不模拟AF
	set_ZF(res, data_size);
	set_SF(res, data_size); 
	set_OF_add(res, src, dest, data_size);
	
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	uint32_t res = 0; 
	uint32_t tmp = dest + src;
	res = dest + src + cpu.eflags.CF;
	
	set_CF_add(tmp, src, data_size); 
	if (!cpu.eflags.CF) set_CF_add(res, tmp, data_size);
	set_PF(res); 
	// set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size); 
	set_OF_add(res, src, dest, data_size);
	
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
	uint32_t res = 0; 
	res = dest - src;

	set_CF_sub(dest, src, data_size); 
	set_PF(res); 
	// set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size); 
	set_OF_add(res, ~src, dest, data_size);
	
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
	uint32_t res = 0; 
	uint32_t tmp = dest - cpu.eflags.CF;
	res = dest - src - cpu.eflags.CF;
	
	set_CF_sub(dest, cpu.eflags.CF, data_size); 
	if (!cpu.eflags.CF) set_CF_sub(tmp, src, data_size);
	set_PF(res); 
	// set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size); 
	set_OF_add(res, ~src, dest, data_size);
	
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
	uint64_t res = 0;
	res = (uint64_t)dest * (uint64_t)src;
	
	set_CF_OF_mul(res, data_size);
	
	return res & (0xFFFFFFFFFFFFFFFF >> (64 - 2 * data_size));
#endif
}

int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
	int64_t res = 0;
	src = src & (0xFFFFFFFF >> (32 - data_size));
	dest = dest & (0xFFFFFFFF >> (32 - data_size));
	
	int64_t esrc = ((src >> (data_size - 1)) & 1) ? ~src + 1 : src;
	int64_t edest = ((dest >> (data_size - 1)) & 1) ? ~dest + 1 : dest;
	esrc = esrc & (0xFFFFFFFF >> (32 - data_size));
	edest = edest & (0xFFFFFFFF >> (32 - data_size));

	res = esrc * edest;
	if (((src >> (data_size - 1)) & 1) != (dest >> (data_size - 1) & 1))
		res *= -1;

	set_CF_OF_imul(res, data_size);

	return res;
#endif
}

// need to implement alu_mod before testing
uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
	uint32_t res = 0;
	res = dest / src;
	return res;
#endif
}

// need to implement alu_imod before testing
int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
	int32_t res = 0;
	res = dest / src;
	return res;
#endif
}

uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
	uint32_t res = 0;
	res = dest % src;
	return res;
#endif
}

int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
	int32_t res = 0;
	res = dest % src;
	return res;
#endif
}

uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
	uint32_t res = 0;
	res = src & dest;
	
	cpu.eflags.OF = 0;
	cpu.eflags.CF = 0;
	set_PF(res); 
	// set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size); 
	
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
	uint32_t res = 0;
	res = src ^ dest;
	
	cpu.eflags.OF = 0;
	cpu.eflags.CF = 0;
	set_PF(res); 
	// set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size); 
	
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
	uint32_t res = 0;
	res = src | dest;
	
	cpu.eflags.OF = 0;
	cpu.eflags.CF = 0;
	set_PF(res); 
	// set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size); 
	
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
	uint32_t res = 0;
	dest = dest & (0xFFFFFFFF >> (32 - data_size));
	res = dest << src;
	
	set_CF_sal(dest, src, data_size);
	set_PF(res); 
	// set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size); 
	if (src == 1) cpu.eflags.OF = sign(dest) == sign(src);
	
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shr(src, dest, data_size);
#else
	uint32_t res = 0;
	dest = dest & (0xFFFFFFFF >> (32 - data_size));
	res = dest >> src;
	
	set_CF_sar(dest, src, data_size);
	set_PF(res);
	// set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size); 
	cpu.eflags.OF = 0;
	
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sar(src, dest, data_size);
#else
	uint32_t res = 0;
	dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
	res = (int32_t)dest >> src;
	
	set_CF_sar(dest, src, data_size);
	set_PF(res); 
	// set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size); 
	cpu.eflags.OF = 0;
	
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
	uint32_t res = 0;
	dest = dest & (0xFFFFFFFF >> (32 - data_size));
	res = dest << src;
	
	set_CF_sal(dest, src, data_size);
	set_PF(res); 
	// set_AF();
	set_ZF(res, data_size);
	set_SF(res, data_size); 
	if (src == 1) cpu.eflags.OF = sign(dest) == sign(src);
	
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}
