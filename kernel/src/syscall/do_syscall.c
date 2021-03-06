#include "x86.h"

#include <sys/syscall.h>

void add_irq_handle(int, void (*)(void));
void mm_brk(uint32_t);
int fs_open(const char *, int);
ssize_t fs_read(int, uint8_t *, uint32_t);
ssize_t fs_write(int, uint8_t *, uint32_t);
off_t fs_lseek(int, off_t, int);
int fs_close(int);

static void sys_brk(TrapFrame *tf)
{
#ifdef IA32_PAGE
	mm_brk(tf->ebx);
#endif
	tf->eax = 0;
}

static void sys_open(TrapFrame *tf)
{
	//Log("open");
	tf->eax = fs_open((void *)tf->ebx, tf->ecx);
}

static void sys_read(TrapFrame *tf)
{
	//Log("read");
	tf->eax = fs_read(tf->ebx, (void *)tf->ecx, tf->edx);
}

static void sys_write(TrapFrame *tf)
{
	//Log("write");
	tf->eax = fs_write(tf->ebx, (void *)tf->ecx, tf->edx);
}

static void sys_lseek(TrapFrame *tf)
{
	//Log("lseek");
	tf->eax = fs_lseek(tf->ebx, tf->ecx, tf->edx);
}

static void sys_close(TrapFrame *tf)
{
	//Log("close");
	tf->eax = fs_close(tf->ebx);
}

void do_syscall(TrapFrame *tf)
{
	switch (tf->eax)
	{
	case 0:
		cli();
		//Log("\n\tadd_irq_handle\n\tebx: %08x  func: %x  eip: %08x", tf->ebx, tf->ecx, tf->eip);
		add_irq_handle(tf->ebx, (void *)tf->ecx);
		sti();
		break;
	case SYS_brk:
		//Log("\n\tsys_brk\n\tebx: %08x  irq: %x error_code: %x eip: %08x", tf->ebx, tf->irq, tf->error_code, tf->eip);
		sys_brk(tf);
		break;
	case SYS_open:
		sys_open(tf);
		break;
	case SYS_read:
		sys_read(tf);
		break;
	case SYS_write:
		sys_write(tf);
		break;
	case SYS_lseek:
		sys_lseek(tf);
		break;
	case SYS_close:
		sys_close(tf);
		break;
	default:
		panic("Unhandled system call: id = %d", tf->eax);
	}
}
