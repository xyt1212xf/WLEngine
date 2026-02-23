#pragma once
#include "Common.h"
namespace TinyL
{
	enum Op : int8_t
	{
		OP_LOADK,     // R[a] = kst[b]
		OP_LOADV,     // R[a] = vars[b]
		OP_STORE,     // vars[a] = R[b]
		OP_ADD,       // R[a] = R[b] + R[c]
		OP_SUB,
		OP_MUL,
		OP_LT,        // R[a] = R[b] < R[c]  (0/1)
		OP_JMP,       // pc += off
		OP_JMPF,      // if R[a]==0  pc += off
		OP_PRINT,     // print R[a]
		OP_NEWTABLE,  // R[a] = {}
		OP_GETTABLE,  // R[a] = R[b][kst[c]]
		OP_SETTABLE,  // R[b][kst[c]] = R[a]
		OP_CALL,      // R[a] = call(R[b], #arg=R[c])
		OP_RETURN,    // return R[a]
		OP_HALT
	};
	using Number = double;
	struct Instr 
	{
		Op op; 
		uint8_t a, b, c; 
		int16_t off; 
	};

	enum VTag 
	{ 
		V_NIL,
		V_NUM, 
		V_TAB
	};
	struct Value 
	{
		VTag tag = V_NIL;
		double num = 0;
		std::unordered_map<std::string, Value> tab;   // 仅当 V_TAB 有效
		Value() = default;
		Value(double n)
		: tag(V_NUM), num(n) 
		{
			
		}
		static Value table() { Value v; v.tag = V_TAB; return v; }
		bool isNil() const { return tag == V_NIL; }
		bool isNum() const { return tag == V_NUM; }
		bool isTab() const { return tag == V_TAB; }
		double asNum() const { return num; }
	};
}