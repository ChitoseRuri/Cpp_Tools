#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include "Pl0_Analyze.h"
#include "Pl0_Gramma.h"
#include "Pl0_Lexical.h"


class Pl0
{
private:
	std::string m_FilePath;
	Pl0_Lexical m_Lexical;
	Pl0_Gramma m_Gramma;
	Pl0_Analyze m_Analyze;

private:
	

public:
	Pl0();
	Pl0(std::string path);
	~Pl0();
	void setPath(std::string path);
	void Analyze();
};