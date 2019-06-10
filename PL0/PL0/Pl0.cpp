#include "Pl0.h"

Pl0::Pl0()
{
}

Pl0::Pl0(std::string path)
{
	setPath(std::move(path));
}

Pl0::~Pl0()
{
}

void Pl0::setPath(std::string path)
{
	m_Lexical.openFile(path);
	m_FilePath = std::move(path);
}

void Pl0::Analyze()
{
	while (true)
	{
		auto sym = m_Lexical.getWord().SYM;
		switch (sym)
		{
		case Sym::Error_Word:
			break;
		case Sym::BEGIN:
			break;
		case Sym::END:
			break;
		case Sym::ELSE:
			break;
		case Sym::FOR:
			break;
		case Sym::STEP:
			break;
		case Sym::UNTIL:
			break;
		case Sym::DO:
			break;
		case Sym::RETURN:
			break;
		case Sym::EQUAL:
			break;
		case Sym::ADD:
			break;
		case Sym::MINUS:
			break;
		case Sym::MUTI:
			break;
		case Sym::DIVIDE:
			break;
		case Sym::ADD_EQUAL:
			break;
		case Sym::MINUS_EQUAL:
			break;
		case Sym::MUTI_EQUAL:
			break;
		case Sym::DIVIDE_EQUAL:
			break;
		case Sym::Identifier:
			break;
		case Sym::Number:
			break;
		case Sym::Eof:
		{
			return;
		}
			break;
		default:
			break;
		}
	}
}