#pragma once
#include "Common.h"
#include "Code.h"
namespace TinyL
{
	struct VM 
	{
		VM(const std::vector<Instr>& c, const std::vector<Variable>& k, uint8_t vn)
		: code(c)
		, kst(k) 
		{
			vars.resize(vn);
			stack.resize(10240);
		}
		void run();

		std::vector<Variable> stack;
		std::vector<Variable> vars;
		const std::vector<Instr>& code;
		const std::vector<Variable>& kst;
	};
}