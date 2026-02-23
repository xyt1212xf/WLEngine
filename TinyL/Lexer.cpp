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
			return { TK_EOF, "" };
		}
		if (isdigit(src[i])) 
		{
			size_t st = i;
			while (i < src.size() && isdigit(src[i]))
			{
				++i;
			}
			if (src[i] == '.')
			{
				++i;
			}
			while (i < src.size() && isdigit(src[i]))
			{
				++i;
			}
			return { TK_NUMBER, "", std::stod(src.substr(st, i - st)) };
		}
		if (isalpha(src[i])) 
		{
			size_t st = i;
			while (i < src.size() && isalnum(src[i]))
			{
				++i;
			}
			std::string w = src.substr(st, i - st);
			if (w == "print")
			{
				return { TK_PRINT, w };
			}
			if (w == "while") 
			{
				return { TK_WHILE, w };
			}
			if (w == "do")
			{
				return { TK_DO, w };
			}
			if (w == "end")
			{
				return { TK_END, w };
			}
			if (w == "function") 
			{
				return { TK_FUNCTION, w };
			}
			if (w == "if")
			{
				return { TK_IF, w };
			}
			if (w == "then") 
			{
				return { TK_THEN, w };
			}
			if (w == "else")
			{
				return { TK_ELSE, w };
			}
			if (w == "return")
			{
				return { TK_RETURN, w };
			}
			return { TK_IDENT, w };
		}
		char c = src[i++];
		switch (c) 
		{
		case ';': return { TK_SEMI, "" };
		case '(': return { TK_LP, "" };
		case ')': return { TK_RP, "" };
		case '{': return { TK_LB, "" };
		case '}': return { TK_RB, "" };
		case '+': return { TK_PLUS, "" };
		case '-': return { TK_MINUS, "" };
		case '*': return { TK_STAR, "" };
		case '/': return { TK_SLASH, "" };
		case '<': return { TK_LT, "" };
		case '=': return { TK_EQ, "" };
		case ',': return { TK_COMMA, "" };
		default: std::cerr << "bad char " << c << '\n'; exit(1);
		}
	}
}
