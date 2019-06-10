#pragma once
#include <string>

enum class Sym
{
	Error_Word,
	IF,
	THEN,
	BEGIN,
	END,
	ELSE,
	FOR,
	STEP,
	UNTIL,
	DO,
	RETURN,
	COMMENT,
	EQUAL,
	ADD,
	MINUS,
	MUTI,
	DIVIDE,
	ADD_EQUAL,
	MINUS_EQUAL,
	MUTI_EQUAL,
	DIVIDE_EQUAL,
	


	Identifier,
	Number,
	Eof
};

struct Amount
{
	Sym SYM;
	std::string ID;
	int NUM;
};

const char* const keepWord[]{ "if", "then", "begin", "end", "else", "for", "step", "until", "do", "return" ,
"//",":=", "+","-","*","/","+=","-=","*=","/=" };

const char const throwChar[]{ ' ','	' };

const char const coChar[]{ ':','/' };

enum class CharType
{
	Name,
	CCC,// 可连续字符
	NCC,// 不可连续字符
	Space,
	Error
};