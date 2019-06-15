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
	auto & amount = m_Lexical.getWord();
}