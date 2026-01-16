#pragma once
#include "Common.h"
#include "Code.h"
namespace TinyL
{
	struct VM 
	{
		VM(const std::vector<Instr>& c, const std::vector<Number>& k, uint8_t vn)
		: code(c)
		, kst(k) 
		{
			vars.resize(vn);
			stack.resize(10240);
		}
		void run();

		std::vector<Number> stack;
		std::vector<Number> vars;
		const std::vector<Instr>& code;
		const std::vector<Number>& kst;
	};
}