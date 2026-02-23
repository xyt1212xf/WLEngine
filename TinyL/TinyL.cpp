// TinyL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "TinyL.h"
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	int* p = new int[30];
	p[0] = 1;
	// ❌ UB：人为读取 cookie
	size_t n = *(reinterpret_cast<size_t*>(p) - 1);

	std::cout << "cookie says n = " << n << std::endl;

	delete[] p;
//
//	std::cout << "end of main\n";
//	return 0;
//	std::string src = R"(
//n = 5;
//i = 1;
//while i <= n do
//  print i;
//  i = i + 1;
//end
//
//t = {x=10, y=20};
//print t.x;
//t.z = 30;
//print t.z;
//
//function fact(k)
//  if k <= 1 then return 1; end
//  return k * fact(k - 1);
//end
//print fact(5);
//)";
// 	TinyL::CLexer L(src);
// 	TinyL::CCompiler C(L);
// 	C.compile();
//	TinyL::VM vm(C.getCode(), C.getKst(), C.getVars());
//	vm.run();
	return 0;
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
