#include "main.h"
#define _MoveSize 10
void SetFormSize(unsigned x, unsigned y)
{
	assert(x);
	assert(y);
	std::string command("mode con cols=");
	command.append(std::to_string(x));
	command.append(" lines=");
	command.append(std::to_string(y));
	system(command.c_str());
}

void InputTh(bool * pIsWork, HWND * pTarget)
{
	POINT poi{ 0,0 };
	char text[256]{};

	unsigned lastSize = 0, theSize = 0, i = 0;
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coord{ 0,0 };
	while (*pIsWork)
	{
		::GetCursorPos(&poi);
		ZeroMemory(text, sizeof(text));
		*pTarget = ::WindowFromPoint(poi);
		::GetWindowText(*pTarget, text, 256);
		theSize = strlen(text);
		SetConsoleCursorPosition(handle, coord);
		SetConsoleTextAttribute(handle, 0x07);
		printf_s("Press ");
		SetConsoleTextAttribute(handle, 0x03);
		printf_s("Q");
		SetConsoleTextAttribute(handle, 0x07);
		printf_s(" to lock.\nNow window : %s", text);
		for (i = theSize; i < lastSize; i++)
		{
			putchar(' ');
		}
		lastSize = theSize;
	}
	SetConsoleTextAttribute(handle, 0x07);
	printf_s("\nPress");
	SetConsoleTextAttribute(handle, 0x03);
	printf_s(" 'W','A','S','D'");
	SetConsoleTextAttribute(handle, 0x07);
	printf_s("to move window, ");
	SetConsoleTextAttribute(handle, 0x03);
	printf_s("Q");
	SetConsoleTextAttribute(handle, 0x07);
	printf_s(" to unlock.");
}

int main()
{
	SetConsoleTitle("Catch & Move Window");
	DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_MAXIMIZE, MF_BYCOMMAND);
	//Òþ²Ø¹â±ê
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(hOut, &cci);
	cci.bVisible = false;
	SetConsoleCursorInfo(hOut, &cci);
	//
	while (true)
	{
		char key = 0;
		bool isWork = true;
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		HWND targetHwnd = nullptr;
		SetFormSize(80, 4);
		system("cls");
		SetConsoleTextAttribute(handle, 0x07);
		printf_s("Press ");
		SetConsoleTextAttribute(handle, 0x03);
		printf_s("Q");
		SetConsoleTextAttribute(handle, 0x07);
		printf_s(" to start finding window.");
		while (key != 'q' && key != 'Q')
		{
			if (key == 27)
			{
				exit(0);
			}
			key = _getch();
		}
		key = 0;
		system("cls");
		std::thread th(InputTh, &isWork, &targetHwnd);
		th.detach();
		bool isMove = true;
		while (isMove)
		{
			while (key != 'q' && key != 'Q')
			{
				if (key == 27)
				{
					exit(0);
				}
				key = _getch();
			}
			isWork = false;
			RECT rect;
			while (isMove)
			{
				switch (_getch())
				{
				case 27://ESC
				{
					exit(0);
				}break;
				case 'Q':
				case 'q':
				{
					isMove = false;
				}break;
				case 'W':
				case 'w':
				{
					GetWindowRect(targetHwnd, &rect);
					rect.bottom -= _MoveSize;
					rect.top -= _MoveSize;
					MoveWindow(targetHwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, true);
				}break;
				case 'S':
				case 's':
				{
					GetWindowRect(targetHwnd, &rect);
					rect.bottom += _MoveSize;
					rect.top += _MoveSize;
					MoveWindow(targetHwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, true);
				}break;
				case 'A':
				case 'a':
				{
					GetWindowRect(targetHwnd, &rect);
					rect.left -= _MoveSize;
					rect.right -= _MoveSize;
					MoveWindow(targetHwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, true);
				}break;
				case 'D':
				case 'd':
				{
					GetWindowRect(targetHwnd, &rect);
					rect.left += _MoveSize;
					rect.right += _MoveSize;
					MoveWindow(targetHwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, true);
				}break;
				default:
					break;
				}
			}
		}
	}	
}