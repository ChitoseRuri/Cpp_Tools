#include "Pl0.h"
#include <iostream>
#include <cstdio>
#include <string>

using std::cout;
using std::cin;
using std::string;

int main()
{
	printf_s("Input source file's path:\n");
	char path[256]{};
	cin.getline(path, sizeof(path));
	if (path[0] == '\0')
	{
		sprintf_s(path, "Test/Ìõ¼şÓï¾ä²âÊÔ.text");
	}
	Pl0 pl0;
	pl0.setPath(path);
	pl0.Analyze();
	return 0;
}