// Mutiplay Elevator Simulation.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"


int main()
{
	int a = 20;
	while (a--)
	{
		switch (a%2)
		{
			case 0:
			{

				break;
			}
			case 1:
			{
				continue;
			}
		default:
			break;
		}
		std::cout << a << std::endl;
	}
    return 0;
}

