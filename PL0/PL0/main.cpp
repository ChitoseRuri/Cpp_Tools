#include "Pl0.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <conio.h>

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
		sprintf_s(path, "Test/Ìõ¼şÓï¾ä²âÊÔ.txt");
	}
	Pl0 pl0;
	pl0.setPath(path);
	pl0.Analyze();
	_getch();
	return 0;
}