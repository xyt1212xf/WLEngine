#pragma once
#include <string>
#include "Code.h"

namespace TinyL
{
	enum TkType 
	{
		TK_NUMBER, 
		TK_IDENT, 
		TK_PRINT,
		TK_WHILE, 
		TK_DO, 
		TK_END,
		TK_SEMI, 
		TK_LP, 
		TK_RP,
		TK_PLUS,
		TK_MINUS,
		TK_STAR, 
		TK_SLASH,
		TK_LT,
		TK_EQ,
		TK_EOF,
		TK_ERROR,
	};
	struct Token 
	{
		TkType type; 
		Number num;
		std::string s; 
	};
	class CLexer
	{
	public:
		CLexer(const std::string& s);
		CLexer(std::string&& s);
		Token next();
	private:
		std::string src;
		size_t i = 0;
	};

}

