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
			TkType op = tk.type; 
			nxt();
			uint8_t r = term();
			uint8_t d = reg();
			emit(op == TK_PLUS ? OP_ADD : (op == TK_MINUS ? OP_SUB : OP_LT), d, l, r);
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
			uint8_t r = factor(), d = reg();
			emit(op == TK_STAR ? OP_MUL : OP_SUB, d, f, r);
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
			nxt();
			return t;
		}
		if (tk.type == TK_IDENT) 
		{
			std::string name = tk.s;
			nxt();
			if (tk.type == TK_LP) // 函数调用
			{                       
				match(TK_LP);
				std::vector<uint8_t> args;
				while (tk.type != TK_RP) 
				{
					args.push_back(expr());
					if (tk.type == TK_COMMA)
					{
						nxt();
					}
				}
				match(TK_RP);
				if (vars.count(name) == 0) vars[name] = vars.size();
				uint8_t freg = reg();
				emit(OP_LOADV, freg, vars[name]);
				for (uint8_t a : args) emit(OP_LOADV, reg(), a); // 压参
				uint8_t ret = reg();
				emit(OP_CALL, ret, freg, args.size());
				return ret;
			}
			else if (tk.type == TK_LB)              // 表索引
			{   
				match(TK_LB);
				std::string key = tk.s; match(TK_IDENT); match(TK_RB);
				uint8_t t = reg();
				emit(OP_GETTABLE, t, vars[name], addKStr(key));
				return t;
			}
			else // 普通变量
			{                                      
				if (vars.count(name) == 0) vars[name] = vars.size();
				uint8_t t = reg();
				emit(OP_LOADV, t, vars[name]);
				return t;
			}
		}
		if (tk.type == TK_LB) // 表构造
		{                         
			match(TK_LB);
			uint8_t dst = reg();
			emit(OP_NEWTABLE, dst);
			while (tk.type != TK_RB)
			{
				std::string key = tk.s; match(TK_IDENT); match(TK_EQ);
				uint8_t val = expr();
				emit(OP_SETTABLE, dst, addKStr(key), val);
				if (tk.type == TK_COMMA)
				{
					nxt();
				}
			}
			match(TK_RB);
			return dst;
		}
		std::cerr << "bad factor\n"; exit(1);
	}

	void CCompiler::stmt()
	{
		if (tk.type == TK_PRINT) 
		{
			nxt();
			uint8_t v = expr();
			emit(OP_PRINT, v);
			match(TK_SEMI);
		}
		else if (tk.type == TK_IDENT) 
		{
			std::string name = tk.s;
			nxt();
			if (tk.type == TK_EQ) 
			{
				match(TK_EQ);
				uint8_t v = expr();
				if (vars.count(name) == 0) 
				{
					vars[name] = vars.size();
				}
				emit(OP_STORE, vars[name], v);
				match(TK_SEMI);
			}
			else if (tk.type == TK_LB)// t[k] = expr;
			{               
				match(TK_LB);
				std::string key = tk.s; match(TK_IDENT); match(TK_RB);
				match(TK_EQ);
				uint8_t val = expr();
				if (vars.count(name) == 0) 
				{
					vars[name] = vars.size();
				}
				emit(OP_SETTABLE, vars[name], addKStr(key), val);
				match(TK_SEMI);
			}
			else
			{
				std::cerr << "bad stmt\n"; exit(1);
			}
		}
		else if (tk.type == TK_WHILE) 
		{
			nxt();
			size_t loopHead = code.size();
			uint8_t cond = expr();
			size_t jmpIdx = code.size();
			emit(OP_JMPF, cond, 0, 0, 0);
			match(TK_DO);
			while (tk.type != TK_END) stmt();
			match(TK_END);
			emit(OP_JMP, 0, 0, 0, loopHead - (int)code.size() - 1);
			code[jmpIdx].off = code.size() - jmpIdx - 1;
		}
		else if (tk.type == TK_IF) 
		{
			nxt();
			uint8_t cond = expr();
			size_t jmpFalse = code.size();
			emit(OP_JMPF, cond, 0, 0, 0);
			match(TK_THEN);
			while (tk.type != TK_ELSE && tk.type != TK_END) 
			{
				stmt();
			}
			if (tk.type == TK_ELSE) 
			{
				nxt();
				size_t jmpEnd = code.size();
				emit(OP_JMP, 0, 0, 0, 0);
				code[jmpFalse].off = code.size() - jmpFalse - 1;
				while (tk.type != TK_END) stmt();
				code[jmpEnd].off = code.size() - jmpEnd - 1;
			}
			else 
			{
				code[jmpFalse].off = code.size() - jmpFalse - 1;
			}
			match(TK_END);
		}
		else if (tk.type == TK_FUNCTION) 
		{
			nxt();
			std::string fname = tk.s; match(TK_IDENT);
			match(TK_LP);
			std::vector<std::string> params;
			while (tk.type != TK_RP)
			{
				params.push_back(tk.s);
				match(TK_IDENT);
				if (tk.type == TK_COMMA) 
				{
					nxt();
				}
			}
			match(TK_RP);
			if (vars.count(fname) == 0)
			{
				vars[fname] = vars.size();
			}
			/* 极简：把函数体当原生 print 实现，硬编码编号 999 */
			emit(OP_LOADK, reg(), addK(999)); // 999 代表 print
			emit(OP_STORE, vars[fname], reg() - 1);
			match(TK_END);
		}
		else if (tk.type == TK_RETURN) 
		{
			nxt();
			uint8_t v = expr();
			emit(OP_RETURN, v);
			match(TK_SEMI);
		}
		else 
		{
			std::cerr << "bad stmt\n"; exit(1);
		}
	}

}