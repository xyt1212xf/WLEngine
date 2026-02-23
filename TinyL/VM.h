#pragma once
#include "Common.h"
#include "Code.h"
namespace TinyL
{
	struct VM 
	{
		VM(const std::vector<Instr>& c, const std::vector<Number>& k, int vn)
		: code(c)
		, kst(k) 
		{
			vars.resize(vn);
			stack.resize(256);               // ×ã¹»¼Ä´æÆ÷
		}
		Value pop() 
		{
			Value v = stack.back();
			stack.pop_back();
			return v;
		}
		
		void push(const Value& v) 
		{ 
			stack.push_back(v); 
		}
		void run();

		std::vector<Value> stack, vars;
		const std::vector<Instr>& code;
		const std::vector<Number>& kst;
		int pc = 0;
	};
}