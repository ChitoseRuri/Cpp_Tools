#include "Pl0_Lexical.h"

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
	if (m_FileReader >> m_Amount.NUM)
	{
		m_Amount.SYM = Sym::Number;
	}
	else
	{
		m_Buffer.clear();
		char ch;
		while (true)
		{
			m_FileReader >> ch;
			m_Buffer += ch;
			auto sym = m_SymSearch[m_Buffer];
			if (sym != Sym::Error_Word)
			{
				m_Amount.SYM = sym;
				std::cout << m_Buffer << '\n';
				break;
			}
			if (m_FileReader.eof())
			{
				m_Amount.SYM = Sym::Eof;
				break;
			}
		}
	}
	return m_Amount;
}

