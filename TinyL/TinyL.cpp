// TinyL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "TinyL.h"

int main()
{
	std::string src = R"(
	n = 5;
	i = 1;
	while i < 10 do
	  print i;
	  i = i * 2;
	end
	)";
	TinyL::CLexer L(src);
	TinyL::CCompiler C(L);          // 1. 词法+语法分析
	C.compile();            // 2. 生成字节码 → C.code 被填好

	const auto& opcodes = C.getCode();          // 3. 拿到引用
	const auto& kpool = C.getKst();
	uint8_t gvars = C.getVars();

	TinyL::VM vm(opcodes, kpool, gvars);   // 4. 构造 VM
	vm.run();                       // 5. 执行
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
