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
	INT,
	CHAR,

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

const char* const keepWord[]{ "if", "then", "begin", "end", "else", "for", "step", "until", "do", "return" , "int ", "char", 
"//",":=", "+","-","*","/","+=","-=","*=","/=" };

const char throwChar[]{ ' ','	' };

const char escapeKey[]{ 'a','b','f','n','r','t','v','\\','\'','\"','\?','\0' };
const char escapeValue[]{ 7,8,12,10,13,9,11,92,39,34,63,0 };

enum class CharType
{
	Name,
	CC,
	ES,
	Space,
	Error
};

enum class GrammaError
{
	Succeed,
	Error
};