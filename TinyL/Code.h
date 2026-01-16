#pragma once
#include "Common.h"
namespace TinyL
{
	enum Op : int8_t
	{
		OP_LOADK,  // R[a] = kst[b]
		OP_LOADV,  // R[a] = vars[b]
		OP_STORE,  // vars[a] = R[b]
		OP_ADD,    // R[a] = R[b] + R[c]
		OP_SUB,
		OP_MUL,
		OP_LT,     // R[a] = R[b] < R[c]  (0/1)
		OP_JMP,    // pc += off
		OP_JMPF,   // if R[a]==0  pc += off
		OP_PRINT,  // print R[a]
		OP_HALT
	};
	using Number = double;
	struct Instr 
	{
		Op op; 
		uint8_t a, b, c; 
		int16_t off; 
	};
}