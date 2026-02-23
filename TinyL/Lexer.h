#pragma once
#include <string>
#include "Code.h"

namespace TinyL
{
	enum TkType
	{
		TK_NUMBER,   // 数字字面量，如 123  3.14
		TK_IDENT,    // 标识符/变量名，如 a  foo  bar
		TK_PRINT,    // 关键字 print
		TK_WHILE,    // 关键字 while
		TK_DO,       // 关键字 do
		TK_END,      // 关键字 end
		TK_FUNCTION, // 关键字 function
		TK_IF,       // 关键字 if
		TK_THEN,     // 关键字 then
		TK_ELSE,     // 关键字 else
		TK_RETURN,   // 关键字 return
		TK_SEMI,     // 分号 ;
		TK_LP,       // 左圆括号 (
		TK_RP,       // 右圆括号 )
		TK_LB,       // 左花括号 {
		TK_RB,       // 右花括号 }
		TK_PLUS,     // 加号 +
		TK_MINUS,    // 减号 -
		TK_STAR,     // 乘号 *
		TK_SLASH,    // 除号 /
		TK_LT,       // 小于号 <
		TK_EQ,       // 等号 =
		TK_COMMA,    // 逗号 ,
		TK_EOF       // 文件结束标记
	};


	struct Token 
	{
		TkType type; 
		std::string s; 
		Number num = 0; 
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

