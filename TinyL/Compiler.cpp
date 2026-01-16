#include "Compiler.h"
namespace TinyL
{
	CCompiler::CCompiler(CLexer& l)
	: L(l) 
	{
		nxt();
	}

	uint8_t CCompiler::expr()
	{
		uint8_t l = term();
		while (tk.type == TK_PLUS || tk.type == TK_MINUS || tk.type == TK_LT)
		{
			TkType op = tk.type; nxt();
			uint8_t r = term();
			uint8_t d = reg();
			if (op == TK_PLUS)
			{
				emit(OP_ADD, d, l, r);
			}
			else if (op == TK_MINUS)
			{
				emit(OP_SUB, d, l, r);
			}
			else if (op == TK_LT)
			{
				emit(OP_LT, d, l, r);
			}
			l = d;
		}
		return l;
	}

	uint8_t CCompiler::term()
	{
		uint8_t f = factor();
		while (tk.type == TK_STAR || tk.type == TK_SLASH)
		{
			TkType op = tk.type; nxt();
			uint8_t r = factor();
			uint8_t d = reg();
			if (op == TK_STAR)
			{
				emit(OP_MUL, d, f, r);
			}
			if (op == TK_SLASH)
			{
			} // ÂÔ
			f = d;
		}
		return f;
	}

	uint8_t CCompiler::factor()
	{
		if (tk.type == TK_NUMBER)
		{
			uint8_t t = reg();
			emit(OP_LOADK, t, addK(tk.num));
			nxt(); return t;
		}
		else if (tk.type == TK_IDENT)
		{
			std::string name = tk.s; nxt();
			if (vars.count(name) == 0) vars[name] = vars.size();
			uint8_t t = reg();
			emit(OP_LOADV, t, vars[name]);
			return t;
		}
		else if (tk.type == TK_LP)
		{
			nxt();
			uint8_t t = expr();
			match(TK_RP);
			return t;
		}
		std::cerr << "bad factor\n"; exit(1);
	}

	void CCompiler::statement()
	{
		if (tk.type == TK_PRINT) 
		{
			nxt();
			uint8_t t = expr();
			emit(OP_PRINT, t);
			match(TK_SEMI);
		}
		else if (tk.type == TK_IDENT) 
		{
			std::string name = tk.s; 
			nxt();
			match(TK_EQ);
			uint8_t t = expr();
			if (vars.count(name) == 0) 
			{
				vars[name] = vars.size();
			}
			emit(OP_STORE, vars[name], t);
			match(TK_SEMI);
		}
		else if (tk.type == TK_WHILE) 
		{
			nxt();
			size_t loopHead = code.size();
			uint8_t cond = expr();
			size_t jmpIdx = code.size();
			emit(OP_JMPF, cond, 0, 0, 0); // off ÉÔºó²¹
			match(TK_DO);
			while (tk.type != TK_END)
			{
				statement();
			}
			match(TK_END);
			emit(OP_JMP, 0, 0, 0, loopHead - (int)code.size() - 1);
			code[jmpIdx].off = code.size() - jmpIdx - 1; // »ØÌî
		}
		else 
		{
			std::cerr << "bad stmt\n"; exit(1); 
		}
	}

}