#pragma once
#include "Pl0_Protocol.h"

class Pl0_Gramma
{
private:
	

public:
	Pl0_Gramma();
	~Pl0_Gramma();

	GrammaError addWord(const Sym & sym);
};