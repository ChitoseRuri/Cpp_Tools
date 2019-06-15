#pragma once
#include <string>
#include <fstream>
#include <unordered_map>
#include <iostream>
#include "Pl0_Protocol.h"

class Pl0_Lexical
{
private:
	std::string m_FilePath;				// �ļ�·��
	std::fstream m_FileReader;			//
	Amount m_Amount;					// 3��ȫ����
	std::string m_Buffer;				// ������
	std::unordered_map<std::string, Sym> m_SymSearch;
	std::unordered_map<char, char> m_EscapeString;

private:
	bool isSpace(char ch);
	CharType checkChar(char ch);

public:
	Pl0_Lexical();
	~Pl0_Lexical();

	void openFile(std::string path);
	void closeFile();
	const Amount& getWord();
	void jumpLine();

};

