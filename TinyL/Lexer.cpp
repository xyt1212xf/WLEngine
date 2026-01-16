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
			return { TK_EOF, Variable(), ""};
		}
		if (isdigit(src[i])) 
		{
			size_t st = i; 
			while (i < src.size() && isdigit(src[i]))
			{
				++i;
			}
			return { TK_NUMBER, Variable(std::stod(src.substr(st,i - st))), ""};
		}
		if (isalpha(src[i])) 
		{
			size_t st = i;
			while (i < src.size() && isalnum(src[i]))
			{ 
				++i;
			}
			std::string w = src.substr(st, i - st);
			if (w == "print") return { TK_PRINT,Variable(),w};
			if (w == "while") return { TK_WHILE,Variable(),w};
			if (w == "do")    return { TK_DO,Variable(),w };
			if (w == "end")   return { TK_END,Variable(),w };
			return { TK_IDENT,Variable(),w };
		}
		char c = src[i++];
		switch (c) 
		{
		case ';': return { TK_SEMI,Variable(),""};
		case '(': return { TK_LP,Variable(),""};
		case ')': return { TK_RP,Variable(),""};
		case '+': return { TK_PLUS,Variable(),""};
		case '-': return { TK_MINUS,Variable(),""};
		case '*': return { TK_STAR,Variable(),""};
		case '/': return { TK_SLASH,Variable(),""};
		case '<': return { TK_LT, Variable(), ""};
		case '=': return { TK_EQ, Variable(), ""};
		}
		return { TK_ERROR, Variable(), "" };
	}
}
