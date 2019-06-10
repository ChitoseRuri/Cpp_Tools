#include "Pl0_Lexical.h"

using std::string;

bool Pl0_Lexical::isSpace(char ch)
{
	for (auto& var : throwChar)
	{
		if (ch == var)
		{
			return true;
		}
	}
	return false;
}

CharType Pl0_Lexical::checkChar(char ch)
{
	if (ch < 0)
	{
		return CharType::Error;
	}
	if (isSpace(ch))
	{
		return CharType::Space;
	}
	if (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_')
	{
		return CharType::Name;
	}
	for (auto& var : coChar)
	{
		if (ch == var)
		{
			return CharType::CCC;
		}
	}
	return CharType::NCC;
}

Pl0_Lexical::Pl0_Lexical()
{
	int i = 0;
	for (auto str : keepWord)
	{
		m_SymSearch.insert(std::make_pair(str, static_cast<Sym>(++i)));
	}
}


Pl0_Lexical::~Pl0_Lexical()
{
}

void Pl0_Lexical::openFile(std::string path)
{
	m_FilePath = std::move(path);
	m_FileReader.close();
	m_FileReader.open(m_FilePath.c_str());
	char ch = -1;
	while (ch < 0)
	{
		m_FileReader >> ch;
	}
	m_FileReader.seekg(-1, std::ios::cur);
}

void Pl0_Lexical::closeFile()
{
	m_FileReader.close();
}

const Amount& Pl0_Lexical::getWord()
{
	if (m_FileReader.eof())
	{
		m_Amount.SYM = Sym::Eof;
		return m_Amount;
	}
	// Ìø¹ý¿Õ°××Ö·û
	char ch = ' ';
	while (isSpace(ch))
	{
		if (m_FileReader.eof())
		{
			m_Amount.SYM = Sym::Eof;
			return m_Amount;
		}
		m_FileReader >> ch;
	}
	// ÅÐ¶Ï×Ö·ûÊÇ·ñÎªÊý×Ö
	if (ch >= '0' && ch <= '9')
	{
		m_FileReader.seekg(-1, std::ios::cur);
		m_FileReader >> m_Amount.NUM;
		m_Amount.SYM = Sym::Number;
	}
	else // ÅÐ¶Ï±£Áô×Ö 
	{
		m_Buffer.clear();
		auto type = checkChar(ch);
		while (true)
		{
			m_Buffer += ch;
			if (m_FileReader.eof())
			{
				if (type != checkChar(ch))
				{
					m_FileReader.seekg(-1, std::ios::cur);
				}
				m_Amount.SYM = m_SymSearch[m_Buffer];
				std::cout << m_Buffer;
				break;
			}
			m_FileReader >> ch;
		}
	}
	return m_Amount;
}

void Pl0_Lexical::jumpLine()
{
	string ts;
	std::getline(m_FileReader, ts);
}