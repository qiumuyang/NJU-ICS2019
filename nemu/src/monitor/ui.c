#include "nemu.h"
#include "monitor/ui.h"
#include "monitor/breakpoint.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "memory/mmu/page.h"
#include "memory/cache.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

extern void list_sym();

void list_cache()
{
	bool val = false;
	for (int i = 0; i < CACHE_GROUP; i++)
	{
		bool prt = false;
		for (int j = 0; j < CACHE_LINE; j++)
		{
			if (cache[i][j].valid)
			{
				prt = true;
				val = true;
				break;
			}
		}
		if (!prt) continue;
		printf("Group %d\n", i);
		for (int j = 0; j < CACHE_LINE; j++)
			if (cache[i][j].valid)
				printf("\tLine: %d Tag: %04x\n",j, cache[i][j].tag);
	}
	if (!val) printf("No valid cache\n");
}


/* We use the `readline' library to provide more flexibility to read from stdin. */
char *rl_gets()
{
	static char *line_read = NULL;
	if (line_read)
	{
		free(line_read);
		line_read = NULL;
	}
	do
	{
		line_read = readline("(nemu) ");
	} while (!(line_read && *line_read));
	add_history(line_read);
	return line_read;
}

#define cmd_handler(cmd) static int cmd(char *args)

cmd_handler(cmd_c)
{
	// execute the program
	exec(-1);
	return 0;
}

cmd_handler(cmd_q)
{
	// terminate
	return -1;
}

cmd_handler(cmd_si)
{
	if (args == NULL)
	{
		exec(1);
		return 0;
	}
	char *steps = strtok(NULL, " ");
	if (steps == NULL)
	{
		exec(1);
	}
	else
	{
		int n = 1;
		if (sscanf(steps, "%d", &n) == 1 && n > 0)
		{
			exec(n);
		}
		else
		{
			printf("Bad number: \e[0;31m%s\e[0m\n", steps);
		}
	}
	return 0;
}
cmd_handler(cmd_cache)
{
#ifdef CACHE_ENABLED
	if (args == NULL)
	{
		printf("Command format: \"cache [group] [line]\"\n");
		return 0;
	}
	char *arg = strtok(NULL, " ");
	char *arg2 = strtok(NULL, " ");
	if (arg == NULL || arg2 == NULL)
	{
		printf("Command format: \"cache [group] [line]\"\n");
		return 0;
	}
	else
	{
		int group = 0, line = 0;
		if (sscanf(arg, "%d", &group) == 1)
		{
			if (group < 0 || group > CACHE_GROUP)
				printf("Group index out of range: \e[0;31m%s\e[0m\n", arg);
			else
			{
				if (sscanf(arg2, "%d", &line) == 1)
				{
					if (line < 0 || line > CACHE_LINE)
						printf("line index out of range: \e[0;31m%s\e[0m\n", arg2);
					else
					{
						// print info
						if (!cache[group][line].valid)
						{
							printf("Group %d Line %d invalid\n", group, line);
						}
						else
						{
							printf("Tag: %x\n", cache[group][line].tag);
							CacheLine *cl = &(cache[group][line]);
							uint32_t i = 0;
							while (i < CACHE_LINE_SIZE)
							{
								printf("%02x", (*cl).data[i]);
								if (i % 4 == 3) printf(" ");
								if (i % 16 == 15) printf("\n");
								i++;
							}
						}
					}
				}
				else
					printf("Bad number: \e[0;31m%s\e[0m\n", arg2);
			}
		}
		else
			printf("Bad number: \e[0;31m%s\e[0m\n", arg);
	}
#else
	printf("\e[0;31mCache is not enabled\n\e[0m");
#endif
	return 0;
}

cmd_handler(cmd_info)
{
	char *arg = strtok(NULL, " ");
	if (arg == NULL)
	{
		printf("Command format: \"info r\" or \"info w\"\n");
	}
	else
	{
	if (strcmp(arg, "r") == 0)
	{
		print_reg();
	}
	else if (strcmp(arg, "w") == 0)
	{
		list_breakpoint();
	}
	else if (strcmp(arg, "c") == 0)
	{
		list_cache();
	}
	else if (strcmp(arg, "s") == 0)
	{
		list_sym();
	}
	else
	{
		printf("undefined info args\n");
	}
	}
	return 0;
}

extern int err_pos;
// static void cmd_p(char *e, char *cmd_end) {
cmd_handler(cmd_p)
{
	if (args == NULL)
	{
		goto p_error;
	}
	//if(args + strspn(args, " ") >= cmd_end) { goto p_error; }
	bool hex = false;
	if (strncmp(args, "/x ", 3) == 0)
	{
		hex = true;
		args += 3;
	}
	
	bool success = true;
	uint32_t val = expr(args, &success);
	if (!success)
	{
		char tmp[32] = {0};
		strncpy(tmp,args,err_pos);
		printf("invalid expression: '%s\e[0;31m%s\e[0m'\n", tmp, args + err_pos);
	}
	else
	{
		if (hex) printf("0x%x\n", val);
		else printf("%d\n", val);
	}
	*args = 0;
	return 0;

p_error:
	puts("Command format: \"p EXPR\"");
	return 0;
}

cmd_handler(cmd_v2p)
{
	if (args == NULL)
	{
		goto p_error;
	}
	//if(args + strspn(args, " ") >= cmd_end) { goto p_error; }
	if (!cpu.cr0.PE || !cpu.cr0.PG)
	{
		printf("\e[0;31mPage/Seg is not enabled\n\e[0m");
		return 0;
	}
	bool success = true;
	uint32_t val = expr(args, &success);
	if (!success)
	{
		char tmp[32] = {0};
		strncpy(tmp,args,err_pos);
		printf("invalid expression: '%s\e[0;31m%s\e[0m'\n", tmp, args + err_pos);
	}
	else
	{
		printf("vaddr: %08x paddr: %08x\n",val, page_translate(val));
	}
	*args = 0;
	return 0;

p_error:
	puts("Command format: \"vtp EXPR\"");
	return 0;
}

uint32_t look_up_fun_symtab(char *, bool *);

//static void cmd_b(char *e, char *cmd_end) {
cmd_handler(cmd_b)
{
	vaddr_t addr;
	char fun_name[81];
	bool success = true;
	if (args == NULL)
	{
		goto b_error;
	}

	args += strspn(args, " ");
	if (*args == '*')
	{
		args++;
		addr = expr(args, &success);
		if (!success)
		{
			printf("invalid expression: '%s'\n", args);
			return 0;
		}
	}
	else if (sscanf(args, "%80[a-zA-Z0-9_]", fun_name) == 1)
	{
		//addr = look_up_fun_symtab(fun_name, &success) + 3;	// +3 to skip the machine code of "push %ebp; movl %esp, %ebp"
		addr = look_up_fun_symtab(fun_name, &success);
		if (!success)
		{
			printf("function '%s' not found\n", fun_name);
			return 0;
		}
	}
	else
	{
	b_error:
		puts("Command format: \"b *ADDR\" or \"b FUN\"");
		return 0;
	}

	int NO = set_breakpoint(addr);
	printf("set breakpoint %d at address 0x%08x\n", NO, addr);
	return 0;
}

//static void cmd_w(char *e, char *cmd_end) {
cmd_handler(cmd_w)
{
	if (args == NULL)
	{
		puts("Command format: \"w EXPR\"");
		return 0;
	}

	args += strspn(args, " ");
	int NO = set_watchpoint(args);
	if (NO == -1)
	{
		printf("invalid expression: '%s'\n", args);
		return 0;
	}

	printf("set watchpoint %d\n", NO);
	return 0;
}

//static void cmd_d() {
cmd_handler(cmd_d)
{
	char *p = strtok(NULL, " ");
	if (p == NULL)
	{
		char c;
		while (1)
		{
			printf("Delete all breakpoints? (y or n) ");
			scanf("%c", &c);
			switch (c)
			{
			case 'y':
				delete_all_breakpoint();
			case 'n':
				return 0;
			default:
				puts("Please answer y or n.");
			}
		}
	}

	int NO;
	for (; p != NULL; p = strtok(NULL, " "))
	{
		if (sscanf(p, "%d", &NO) != 1)
		{
			printf("Bad breakpoint number: '%s'\n", p);
			return 0;
		}
		delete_breakpoint(NO);
	}
	return 0;
}
cmd_handler(cmd_hbt);
cmd_handler(cmd_hgt);
cmd_handler(cmd_help);

static struct
{
	char *name;
	char *description;
	int (*handler)(char *);
} cmd_table[] = {
	{"b", "Set breakpoint", cmd_b},
	{"c", "Continue the execution of the program", cmd_c},
	{"cache", "Display cache content", cmd_cache},
	{"d", "Delete breakpoint(s)", cmd_d},
	{"exit", "Exit NEMU", cmd_q},
	{"help", "Display informations about all supported commands", cmd_help},
	{"hit_good_trap", "Auto Hit Good Trap", cmd_hgt},
	{"hit_bad_trap", "Auto Hit Bad Trap", cmd_hbt},
	{"info", "Print register, cache, symbol and watch point info", cmd_info},
	{"p", "Evaluate an expression", cmd_p},
	{"q", "Exit NEMU", cmd_q},
	{"si", "Single Step Execution", cmd_si},
	{"vtp", "Translate vaddr to paddr", cmd_v2p},
	{"w", "Set watchpoint", cmd_w}
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))
uint32_t get_trap(bool good)
{
	bool success = true;
	uint32_t begin = look_up_fun_symtab("main", &success);
	if (!success) return 0;
#ifdef IA32_PAGE
	uint32_t diff = page_translate(begin) - begin;
	begin = page_translate(begin);
#else
	uint32_t diff = 0;
#endif
	for (int i = 0; i < 0x200; i++)
	{
		int pos = begin + i;
		if (hw_mem[pos] != 0xb8) continue;
		int count = 1;
		while (hw_mem[pos + count] == 0)
			count++;
		if (good)
		{
			if (count == 5 && hw_mem[pos + count] == 0x82) return pos + count - diff;
		}
		else
		{
			pos++;
			if (count != 1 || hw_mem[pos] != 1) continue;
			while (hw_mem[pos + count] == 0)
				count++;
			if (count == 4 && hw_mem[pos + count] == 0x82) return pos + count - diff;
		}
	}
	return 0;
}

cmd_handler(cmd_hbt)
{
#ifdef IA32_PAGE
	
	exec(382000);
	
#endif
	/*if (cpu.eip < 0x08000000)
	{
		printf("use command si until eip goes beyond 0x08000000\n");
		return 0;
	}*/
	uint32_t eip = get_trap(0);
	printf("nemu: HIT \e[0;31mBAD\e[0m TRAP at eip = 0x%08x\nNEMU2 terminated\n", eip);
	return -1;
}
cmd_handler(cmd_hgt)
{
#ifdef IA32_PAGE
	
	exec(382000);
	
#endif
	/*if (cpu.eip < 0x08000000)
	{
		printf("use command si until eip goes beyond 0x08000000\n");
		return 0;
	}*/
	uint32_t eip = get_trap(1);
	printf("nemu: HIT \e[0;32mGOOD\e[0m TRAP at eip = 0x%08x\nNEMU2 terminated\n", eip);
	return -1;
}

cmd_handler(cmd_help)
{
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if (arg == NULL)
	{
		/* no argument given */
		for (i = 0; i < NR_CMD; i++)
		{
			printf("%-11s\t-\t%s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else
	{
		for (i = 0; i < NR_CMD; i++)
		{
			if (strcmp(arg, cmd_table[i].name) == 0)
			{
				printf("%-11s\t-\t%s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command \e[0;31m%s\e[0m\n", arg);
	}
	return 0;
}

// the main loop of accepting user commands
void ui_mainloop(bool autorun)
{

	if (autorun)
	{
		cmd_c("");
		if (nemu_state == NEMU_STOP)
		{
			return;
		}
	}

	while (true)
	{
		// read user command
		char *user_cmd = rl_gets();
		char *user_cmd_end = user_cmd + strlen(user_cmd);
		// parse the command
		char *cmd = strtok(user_cmd, " ");
		if (cmd == NULL)
		{
			// empty command, should not happen
			continue;
		}

		// the remaining part of user_cmd is considered as arguments
		char *args = user_cmd + strlen(cmd) + 1;
		args += strspn(args, " ");
		if (args >= user_cmd_end)
		{
			// there is no argument
			args = NULL;
		}

		// execute the command by calling corresponding function
		int i;
		for (i = 0; i < NR_CMD; i++)
		{
			if (strcmp(cmd, cmd_table[i].name) == 0)
			{
				if (cmd_table[i].handler(args) < 0)
				{
					// quit the ui mainloop
					return;
				}
				// stop searching for cmd and wait for the next user_cmd
				break;
			}
		}

		if (i == NR_CMD)
		{
			printf("Unknown user command \e[0;31m%s\e[0m\n", user_cmd);
		}

		if (nemu_state == NEMU_STOP)
		{
			break;
		}
	}
}
