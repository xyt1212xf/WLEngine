#pragma once
#include <vector>
#include <map>
#include "Lexer.h"

namespace TinyL
{
	class CCompiler 
	{
	public:
		CCompiler(CLexer& l) ;
		void compile() 
		{
			while (tk.type != TK_EOF)
			{
				statement();
			}
			emit(OP_HALT);
		}
		std::vector<Instr> getCode()const 
		{
			return code; 
		}
		std::vector<Number> getKst()const 
		{
			return kst; 
		}
		uint8_t getVars()const 
		{ 
			return vars.size();
		}

	private:
		uint8_t expr();

		uint8_t term();

		uint8_t factor(); 
		void statement();
		void nxt() 
		{
			tk = L.next();
		}
		void match(TkType t) 
		{
			if (tk.type != t)
			{
				std::cerr << "syntax\n";
				exit(1); 
			} 
			nxt();
		}
		uint8_t addK(Number v) 
		{
			kst.push_back(v); 
			return kst.size() - 1; 
		}
		uint8_t reg() 
		{
			return R++; 
		}

		void emit(Op op, uint8_t a = 0, uint8_t b = 0, uint8_t c = 0, int16_t off = 0) 
		{
			code.push_back({ op,a,b,c,off });
		}

	private:
		CLexer& L; 
		Token tk;
		std::vector<Instr> code;
		std::vector<Number> kst;
		std::map<std::string, uint8_t> vars;  // 全局变量表
		uint8_t R = 0;			              // 下一个可用寄存器
	};
}