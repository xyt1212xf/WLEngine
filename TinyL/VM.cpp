#include "VM.h"
namespace TinyL
{
	void VM::run()
	{
		size_t pc = 0;
		while (true)
		{
			Instr I = code[pc++];
			switch (I.op)
			{
			case OP_LOADK:
			{
				stack[I.a] = kst[I.b];
			}
			break;
			case OP_LOADV:
			{
				stack[I.a] = vars[I.b];
			}
			break;
			case OP_STORE:
			{
				vars[I.a] = stack[I.b];
			}
			break;
			case OP_ADD:
			{
				stack[I.a].d = stack[I.b].d + stack[I.c].d;
			}
			break;
			case OP_SUB:
			{
				stack[I.a].d = stack[I.b].d - stack[I.c].d;
			}
			break;
			case OP_MUL:
			{
				stack[I.a].d = stack[I.b].d * stack[I.c].d;
			}
			break;
			case OP_LT:
			{
				stack[I.a].d = stack[I.b].d < stack[I.c].d;
			}
			break;
			case OP_JMP:
			{
				pc += I.off;
			}
			break;
			case OP_JMPF:
			{
				if (stack[I.a].d == 0)
				{
					pc += I.off;
				}
			}
			break;
			case OP_PRINT:
			{
				std::cout << stack[I.a].d << '\n';
			}
			break;
			case OP_HALT:
			{
				return;
			}
			}
		}
	}
}