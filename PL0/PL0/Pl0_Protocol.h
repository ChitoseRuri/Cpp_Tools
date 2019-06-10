#pragma once
#include <string>

enum class Sym
{
	Error_Word,
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

const char* const keepWord[]{ "begin", "end", "else", "for", "step", "until", "do", "return" ,
							"//","=","+","-","*","/","+=","-=","*=","/=" };