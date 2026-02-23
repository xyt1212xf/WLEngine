#include "VM.h"
namespace TinyL
{
	void VM::run()
	{
		while (true) 
		{
			Instr I = code[pc++];
			switch (I.op) {
			case OP_LOADK:   stack[I.a] = Value(kst[I.b]); break;
			case OP_LOADV:   stack[I.a] = vars[I.b]; break;
			case OP_STORE:   vars[I.a] = stack[I.b]; break;
			case OP_ADD:     stack[I.a] = Value(stack[I.b].asNum() + stack[I.c].asNum()); break;
			case OP_SUB:     stack[I.a] = Value(stack[I.b].asNum() - stack[I.c].asNum()); break;
			case OP_MUL:     stack[I.a] = Value(stack[I.b].asNum() * stack[I.c].asNum()); break;
			case OP_LT:      stack[I.a] = Value(stack[I.b].asNum() < stack[I.c].asNum()); break;
			case OP_JMP:     pc += I.off; break;
			case OP_JMPF:    if (stack[I.a].asNum() == 0) pc += I.off; break;
			case OP_PRINT:   std::cout << stack[I.a].asNum() << '\n'; break;
			case OP_NEWTABLE:stack[I.a] = Value::table(); break;
			case OP_GETTABLE: {
				auto& tab = stack[I.b].tab;
				std::string key = std::to_string((int)kst[I.c]);
				stack[I.a] = tab.count(key) ? tab[key] : Value();
				break;
			}
			case OP_SETTABLE: {
				auto& tab = stack[I.b].tab;
				std::string key = std::to_string((int)kst[I.c]);
				tab[key] = stack[I.a];
				break;
			}
			case OP_CALL: { /* ½öÖ§³Ö print */
				if (stack[I.b].asNum() == 999)
				{
					std::cout << stack.back().asNum() << '\n';
				}
				stack[I.a] = Value(0);
				break;
			}
			case OP_RETURN:  return;
			case OP_HALT:    return;
			}
		}
	}
}