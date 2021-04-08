#include "nemu.h"
#include "cpu/reg.h"
#include "memory/memory.h"
#include "cpu/cpu.h"

#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>

uint32_t hw_mem_read(paddr_t paddr, size_t len);
void to_lower(char str[]);
uint32_t look_up_symtab(char *, bool *);
uint32_t look_up_fun_symtab(char *, bool *);
bool err_msg_print;
int err_pos;

enum
{
	NOTYPE = 256,
	EQ,
	NEQ,
	LE,
	GE,
	L,
	G,
	AND,
	OR,
	NOT,
	NUM_DEC,
	NUM_HEX,
	NUM_BIN,
	REG32,
	REG16,
	REG8,
	CREG0,
	CREG3,
	SYMB,
	DEREF,
	NEG

	/* TODO: Add more token types */

};

static struct rule
{
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +", NOTYPE}, // white space
	{"\\+", '+'},
	{"-", '-'},
	{"-", NEG},
	{"\\*", '*'},
	{"\\*", DEREF},
	{"\\/", '/'},
	{"\\%", '%'},
	{"\\(", '('},
	{"\\)", ')'},
	{"==", EQ},
	{"!=", NEQ},
	{"<=", LE},
	{">=", GE},
	{"<", L},
	{">", G},
	{"&&", AND},
	{"\\|\\|", OR},
	{"!", NOT},
	{"&", '&'},
	{"\\|", '|'},
	{"\\^", '^'},
	{"~", '~'},
	{"0x[0-9A-Fa-f]{1,8}", NUM_HEX},
	{"[01]{1,32}b", NUM_BIN},
	{"[0-9]{1,10}", NUM_DEC},
	{"\\$([Ee][AaBbCcDd][Xx]|[Ee][BbSsIi][Pp]|[Ee][DdSs][Ii])", REG32},
	{"\\$([AaBbCcDd][Xx]|[BbSs][Pp]|[DdSs][Ii])", REG16},
	{"\\$[abcdABCD][HhLl]", REG8},
	{"\\$[cC][rR]0", CREG0},
	{"\\$[cC][rR]3", CREG3},
	{"[a-zA-Z_][a-zA-Z0-9_]{0,}",SYMB}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX];

/* Rules are used for more times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
	int i;
	char error_msg[128];
	int ret;

	for (i = 0; i < NR_REGEX; i++)
	{
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if (ret != 0)
		{
			regerror(ret, &re[i], error_msg, 128);
			assert(ret != 0);
		}
	}
}



typedef struct token
{
	int type;
	int priority;
	int pos;
	char str[32];
} Token;

Token tokens[32];
int nr_token;
static char *exprs;

static bool make_token(char *e)
{
	exprs = e;
	int position = 0;
	int i;
	regmatch_t pmatch;

	nr_token = 0;

	while (e[position] != '\0')
	{
		/* Try all rules one by one. */
		for (i = 0; i < NR_REGEX; i++)
		{
			if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
			{
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;
				int token_pos = position;
				//printf("match regex[%d] at position %d with len %d: %.*s", i, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. 
				 * Add codes to perform some actions with this token.
				 */

				switch (rules[i].token_type)
				{
				case NOTYPE:
						break;
				default:
					tokens[nr_token].pos = token_pos;
					strncpy(tokens[nr_token].str, substr_start, substr_len);
					tokens[nr_token].str[substr_len] = 0;
					if (tokens[nr_token].type != SYMB) to_lower(tokens[nr_token].str);
					tokens[nr_token].type = rules[i].token_type;
					nr_token++;
				}

				break;
			}
		}

		if (i == NR_REGEX)
		{
			err_pos = position;
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true;
}

/* assist */
void to_lower(char str[])
{
	char *s = str;
	while (*s)
	{
		if (*s >= 'A' && *s <= 'Z')
			*s = *s - 'A' + 'a';
		s++;
	}
}
bool is_single_opr(int op)
{
	return tokens[op].type == DEREF || tokens[op].type == NEG || tokens[op].type == NOT || tokens[op].type == '~';
}
bool is_operand(int id)
{
	return tokens[id].type == NUM_BIN || tokens[id].type == NUM_DEC 
		|| tokens[id].type == NUM_HEX || tokens[id].type == REG32 
		|| tokens[id].type == SYMB || tokens[id].type == REG16
		|| tokens[id].type == REG8 || tokens[id].type == CREG0
		|| tokens[id].type == CREG3;
}
bool is_operator(int id)
{
	return !is_operand(id) && !(tokens[id].type == '(' || tokens[id].type == ')');
}
int __atoi(char *s, int radix)
{
	int res = 0;
	if (radix != 2)
	{
		while (*s)
		{
			res *= radix;
			res += (*s >= '0' && *s <= '9' ? *s - '0' : ((*s >= 'a' ? *s - 'a' : *s - 'A') + 10));
			s++;
		}
	}
	else
	{
		while (*s >= '0' && *s <= '1')
		{
			res *= radix;
			res += *s - '0';
			s++;
		}
	}
	return res;
}
bool check_parentheses(int st, int ed)
{
	if (st > ed) return false;
	int left = 0;
	for (int i = st; i <= ed; i++)
	{
		if (tokens[i].type == '(')
			left++;
		if (tokens[i].type == ')')
		{
			if (left == 0) 
			{
				return false;
			}
			left--;
		}
		if (tokens[i].priority != -1) tokens[i].priority -= 20 * left;
	}
	return left == 0;
}
bool rm_bracket(int st, int ed)
{
	if (tokens[st].type != '(') return false;
	if (tokens[ed].type != ')') return false;
	int left = 1;
	for (int i = st + 1; i < ed; i++)
	{
		if (left < 1) return false;
		if (tokens[i].type == '(')
			left++;
		if (tokens[i].type == ')')
			left--;
	}
	return left >= 1;
}

uint32_t eval(int st, int ed, bool *success)
{
	if (*success == false) return 0;
	if (st > ed)
	{
		*success = false;
		return 0;
	}
	else if (st == ed)
	{
		bool found = false;
		uint32_t res = 0;
		switch (tokens[st].type)
		{
			case NUM_DEC: return __atoi(tokens[st].str, 10);
			case NUM_BIN: return __atoi(tokens[st].str, 2);
			case NUM_HEX: return __atoi(tokens[st].str + 2, 16);
			case CREG0:
#ifdef IA32_SEG
			return cpu.cr0.val;
#else
			printf("IA32_SEG disabled, cr0 treated as 0\n");
			return 0;
#endif
			case CREG3:
#ifdef IA32_PAGE
			return cpu.cr3.val;
#else
			printf("IA32_PAGE disabled, cr3 treated as 0\n");
			return 0;
#endif
			case REG32:
			switch (tokens[st].str[2])
			{
				case 'a':
					return cpu.eax;
				case 'b':
					if (tokens[st].str[3]=='p') 
						return cpu.ebp;
					else if (tokens[st].str[3]=='x') 
						return cpu.ebx;
					else { *success = false; return 0; }
				case 'c':
					return cpu.ecx;
				case 'd':
					if (tokens[st].str[3] == 'i')
						return cpu.edi;
					else if (tokens[st].str[3]=='x') 
						return cpu.edx;
					else { *success = false; return 0; }
				case 's':
					if (tokens[st].str[3] == 'i')
						return cpu.esi;
					else if (tokens[st].str[3]=='p') 
						return cpu.esp;
					else { *success = false; return 0; }
				case 'i':
					if (tokens[st].str[3]=='p') 
						return cpu.eip;
					else { *success = false; return 0; }
				default:
					*success = false;
					return 0;
			}
			case REG16:
			switch (tokens[st].str[1])
			{
				case 'a':
					return cpu.eax & 0xFFFF;
				case 'b':
					if (tokens[st].str[2]=='p') 
						return cpu.ebp & 0xFFFF;
					else if (tokens[st].str[2]=='x') 
						return cpu.ebx & 0xFFFF;
					else { *success = false; return 0; }
				case 'c':
					return cpu.ecx & 0xFFFF;
				case 'd':
					if (tokens[st].str[2] == 'i')
						return cpu.edi & 0xFFFF;
					else if (tokens[st].str[2]=='x') 
						return cpu.edx & 0xFFFF;
					else { *success = false; return 0; }
				case 's':
					if (tokens[st].str[2] == 'i')
						return cpu.esi & 0xFFFF;
					else if (tokens[st].str[2]=='p') 
						return cpu.esp & 0xFFFF;
					else { *success = false; return 0; }
				default:
					*success = false;
					return 0;
			}
			case REG8:
				if (tokens[st].str[1] == 'h') {		// high
					switch (tokens[st].str[0])
					{
						case 'a':
							return (cpu.eax & 0xFF00) >> 8;
						case 'b':
							return (cpu.ebx & 0xFF00) >> 8;
						case 'c':
							return (cpu.ecx & 0xFF00) >> 8;
						case 'd':
							return (cpu.edx & 0xFF00) >> 8;
						default:
							*success = false;
							return 0;
					}
				}
				else {								// low
					switch (tokens[st].str[0])
					{
						case 'a':
							return cpu.eax & 0xFF;
						case 'b':
							return cpu.eax & 0xFF;
						case 'c':
							return cpu.eax & 0xFF;
						case 'd':
							return cpu.eax & 0xFF;
						default:
							*success = false;
							return 0;
					}
				}
			case SYMB: 		// TODO
				res = look_up_symtab(tokens[st].str, &found);
				if (found) return res;
				res = look_up_symtab(tokens[st].str, &found);
				if (found) return res;
				if (err_msg_print)
				{
					err_pos = tokens[st].pos;
					err_msg_print = false;
					printf("symbol %s not found at position %d\n%s\n%*.s^\n", tokens[st].str, tokens[st].pos, exprs, tokens[st].pos, "");
				}
				//for (int i = 0; i < nr_token; i++)
				//	printf("%d: %s\n", i, tokens[i].str);
				*success = false;
				return 0;
			default: *success = false; return 0;
		}
	}
	else if (rm_bracket(st, ed))
		return eval(st + 1, ed - 1, success);
	else
	{
		int op = st;
		while (op <= ed && is_operand(op)) op++;
		if (op == ed)
		{
			*success = false;
			return 0;
		}

		for (int i = st; i <= ed; i++)
			if (tokens[i].priority >= tokens[op].priority && tokens[i].priority != -1) op = i;
		
		uint32_t val1 = 0, val2 = 0;
		if (!is_single_opr(op)) val1 = eval(st, op - 1, success);
		if (!success) return 0;

		val2 = eval(op + 1, ed, success);
		if (!success) return 0;

		switch (tokens[op].type)
		{
			case '~':
				return ~val2;
			case NOT:
				return !val2;
			case DEREF:
				//return paddr_read(val2, 4);
				return vaddr_read(val2, SREG_DS, 4);
			case NEG:
				return -1 * val2;
			case '*':
				return val1 * val2;
			case '/':
				if (val2 == 0) {
				if (err_msg_print)
				{
					err_pos = tokens[op + 1].pos;
					err_msg_print = false; 
					printf("divide 0 error at position %d\n%s\n%*.s^\n", tokens[op + 1].pos, exprs, tokens[op + 1].pos, "");
				}
				*success = false;
				return 0;
				}
				return val1 / val2;
			case '%':
				if (val2 == 0) { 
				if (err_msg_print)
				{
					err_pos = tokens[op + 1].pos;
					err_msg_print = false;
					printf("divide 0 error at position %d\n%s\n%*.s^\n", tokens[op + 1].pos, exprs, tokens[op + 1].pos, "");
				}
				*success = false;
				return 0;
				}
				return val1 % val2;
			case '+':
				return val1 + val2;
			case '-':
				return val1 - val2;
			case LE:
				return val1 <= val2;
			case GE:
				return val1 >= val2;
			case L:
				return val1 < val2;
			case G:
				return val1 > val2;
			case EQ:
				return val1 == val2;
			case NEQ:
				return val1 != val2;
			case '&':
				return val1 & val2;
			case '|':
				return val1 | val2;
			case '^':
				return val1 ^ val2;
			case AND:
				return val1 && val2;
			case OR:
				return val1 || val2;
			default:
				if (err_msg_print)
				{
					err_pos = tokens[op].pos;
					err_msg_print = false;
					printf("invalid operator %s at position %d\n%s\n%*.s^\n", tokens[op].str, tokens[op].pos, exprs, tokens[op].pos, "");
				}
				*success = false;
				return 0;
		}
	}
}

uint32_t expr(char *e, bool *success)
{
	err_msg_print = true;
	if (!make_token(e))
	{
		*success = false;
		return 0;
	}
	for (int i = 0; i < nr_token; i++)
	{
		switch (tokens[i].type)
		{
			case NOT: case '~':
				tokens[i].priority = 2; break;
			case '*': case '/': case '%':
				tokens[i].priority = 3; break;
			case '+': case '-':
				tokens[i].priority = 4; break;
			case LE: case GE: case L: case G:
				tokens[i].priority = 6; break;
			case EQ: case NEQ:
				tokens[i].priority = 7; break;
			case '&':
				tokens[i].priority = 8; break;
			case '|':
				tokens[i].priority = 9; break;
			case '^':
				tokens[i].priority = 10; break;
			case AND:
				tokens[i].priority = 11; break;
			case OR:
				tokens[i].priority = 12; break;
			default:
				tokens[i].priority = -1; break;
		}
		if (tokens[i].type == '*' && (i == 0 || !(is_operand(i - 1) || tokens[i - 1].type == ')')))
		{
			tokens[i].type = DEREF; 
			tokens[i].priority = 2;
		}
		if (tokens[i].type == '-' && (i == 0 || !(is_operand(i - 1) || tokens[i - 1].type == ')')))
		{
			tokens[i].type = NEG;
			tokens[i].priority = 2;
		}
		if (i != 0 && (is_operand(i) && is_operand(i - 1)))
		{
			if (err_msg_print)
			{
				err_msg_print = false;
				err_pos = tokens[i].pos;
				printf("operand error %s at position %d\n%s\n%*.s^\n", tokens[i].str, tokens[i].pos, exprs, tokens[i].pos, "");
			}
			*success = false;		
			return 0;
		}
	}

	if (!check_parentheses(0, nr_token - 1))
	{
		if (err_msg_print)
		{
			err_msg_print = false;
			printf("parentheses error\n");
		}
		*success = false; return 0;
	}
	

	return eval(0, nr_token - 1, success);
}
