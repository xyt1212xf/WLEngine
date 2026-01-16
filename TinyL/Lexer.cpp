#include "Lexer.h"
namespace TinyL
{
	CLexer::CLexer(const std::string& s)
	: src(s)
	{

	}

	CLexer::CLexer(std::string&& s)
	{
		src = s;
	}

	Token CLexer::next()
	{
		while (i < src.size() && isspace(src[i]))
		{
			++i;
		}
		if (i >= src.size())
		{
			return { TK_EOF,0, ""};
		}
		if (isdigit(src[i])) 
		{
			size_t st = i; 
			while (i < src.size() && isdigit(src[i]))
			{
				++i;
			}
			return { TK_NUMBER, std::stod(src.substr(st,i - st)), ""};
		}
		if (isalpha(src[i])) 
		{
			size_t st = i;
			while (i < src.size() && isalnum(src[i]))
			{ 
				++i;
			}
			std::string w = src.substr(st, i - st);
			if (w == "print") return { TK_PRINT,0,w };
			if (w == "while") return { TK_WHILE,0,w };
			if (w == "do")    return { TK_DO,0,w };
			if (w == "end")   return { TK_END,0,w };
			return { TK_IDENT,0,w };
		}
		char c = src[i++];
		switch (c) 
		{
		case ';': return { TK_SEMI,0,""};
		case '(': return { TK_LP,0,""};
		case ')': return { TK_RP,0,""};
		case '+': return { TK_PLUS,0,""};
		case '-': return { TK_MINUS,0,""};
		case '*': return { TK_STAR,0,""};
		case '/': return { TK_SLASH,0,""};
		case '<': return { TK_LT,0, ""};
		case '=': return { TK_EQ,0, ""};
		}
		return { TK_ERROR,0, "" };
	}
}
