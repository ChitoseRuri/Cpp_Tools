#include "ui.h"

CR::Client_UI::Client_UI():m_Handle(GetStdHandle(STD_OUTPUT_HANDLE))
{
	//设置窗体标题
	SetConsoleTitle(CONSOLE_TITLE);
	//移除最大化按钮
	DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_MAXIMIZE, MF_BYCOMMAND);
	//设置窗口大小
	SetFormSize(60, 8);
}

CR::Client_UI::~Client_UI()
{
	CloseHandle(m_Handle);
}

void CR::Client_UI::SetFormSize(_In unsigned x, _In unsigned y)
{
	assert(x);
	assert(y);
	m_FormSizeX = x;
	m_FormSizeY = y;
	string command("mode con cols=");
	command.append(std::to_string(x));
	command.append(" lines=");
	command.append(std::to_string(y));
	system(command.c_str());
}

CR::ControlStatus CR::Client_UI::ConnectMenu(_Out std::string & serverIP)
{
	serverIP.clear();
	SetFormSize(60, 8);
	//绘制界面
	SetConsoleTextAttribute(m_Handle, 0x0f);
	COORD curPos;
	curPos.Y = 4;
	curPos.X = 22;
	SetConsoleCursorPosition(m_Handle, curPos);
	SetConsoleTextAttribute(m_Handle, 0xf3);
	SetConsoleCursorPosition(m_Handle, curPos);
	//读取文件查看上一次的服务器地址
	std::fstream recFile;
	recFile.open("Data/ip.dat");
	string str[4];
	unsigned ip_s[4];
	char key;																	//键盘输出按键
	bool isFileOK = true;
	int strCount;
	//检测保存文件是否损坏
	for (int i = 0; i < 3; i++)
	{
		recFile >> ip_s[i] >> key;
		if (ip_s[i] > 255 || key != '.')
		{
			isFileOK = false;
			break;
		}
		str[i] = std::to_string(ip_s[i]);
	}
	recFile >> ip_s[3];
	if (ip_s[3] > 255)
	{
		isFileOK = false;
	}
	else
	{
		str[3] = std::to_string(ip_s[3]);
	}
	//确认检查结果
	if (isFileOK)
	{
		for (int i = 0; i < 3; i++)
		{
			printf("%3u.", ip_s[i]);
		}
		printf("%3u", ip_s[3]);
		strCount = 3;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			str[i].clear();
		}
		strCount = 0;
	}
	//等待输入
	bool isInput = true;
	while (isInput)
	{
		key = _getch();
		switch (key)
		{
		case '\b':
		{
			if (str[strCount].empty())					//如果该字符串是空的
			{
				if (strCount)										//且不是最后一个字符串
				{
					strCount--;
					printf("\b\b");
					str[strCount].pop_back();
				}
			}
			else
			{
				str[strCount].pop_back();
			}
		}
			break;
		case '\r':
		{
			if (strCount == 3)
			{
				if (str[3].empty())
				{
					str[3] = "0";
				}
				else if (atoi(str[3].c_str()) > 255)
				{
					unsigned length = str[3].length();
					while (length)
					{
						putchar('\b');
						length--;
					}
					str[3] = "255";
					printf("255");
				}
				isInput = false;
			}
		}
			break;
		case '.':
		{
			if (strCount < 3)
			{
				if (str[strCount].empty())
				{
					str[strCount] = "0";
				}
				else if (atoi(str[strCount].c_str()) > 255)
				{
					unsigned length = str[strCount].length();
					while (length)
					{
						putchar('\b');
						length--;
					}
					printf("255");
					str[strCount] = "255";
				}
				putchar('.');
				strCount++;
			}
		}
		default:
		{
			if (key >= '0'&&key <= '9')
			{
				putchar(key);
				str[strCount].push_back(key);
			}
		}
			break;
		}	
	}
	//地址分段整合到serverIP
	for (int add_t = 0; add_t < 4; add_t++)
	{
		serverIP += str[add_t];
		serverIP.push_back('.');
	}
	serverIP.pop_back();
	//把这次的ip地址记录
	recFile << serverIP;
	recFile.close();
	return ControlStatus::SuccessOut;
}

CR::MenuList CR::Client_UI::Main()
{
	SetFormSize(60, 8);
	//隐藏光标
	SetCursorVisible(false);
	SetConsoleTextAttribute(m_Handle, 0x03);
	system("cls");
	//需要画出来的string
	string str_title("Welcome to the Deport~");
	string str_login("Login");
	string str_register("Register");

	unsigned length_title = str_title.length();
	unsigned length_login = str_login.length();
	unsigned length_register = str_register.length();

	//颜色数组
	int color[2]{ 0x03,0xf3 };											//黑底蓝字、白底蓝字

	//清屏，画界面
	system("cls");

	COORD coord;
	coord.X = (m_FormSizeX - length_title) / 2;
	coord.Y = 1;
	SetConsoleCursorPosition(m_Handle, coord);
	SetConsoleTextAttribute(m_Handle, 0x03);
	cout << str_title;
	int color_login = 0, color_register = 1;
	bool isLogin = true;
	while (1)
	{
		coord.X = (m_FormSizeX - length_register) / 2;
		coord.Y = 3;
		SetConsoleCursorPosition(m_Handle, coord);
		SetConsoleTextAttribute(m_Handle, color[static_cast<int>(isLogin)]);
		cout << str_login;

		coord.X = (m_FormSizeX - length_register) / 2;
		coord.Y = 5;
		SetConsoleCursorPosition(m_Handle, coord);
		SetConsoleTextAttribute(m_Handle, color[static_cast<WORD>(!isLogin)]);
		cout << str_register;

		//捕抓键盘事件
		char key = _getch();
		if (key == 9)																																	//按了tab键
		{
			isLogin = !isLogin;
		}
		else if (key == '\r')
		{
			if (isLogin)
			{
				return MenuList::Login;
			}
			else
			{
				return MenuList::Register;
			}
		}
	}
}

CR::ControlStatus CR::Client_UI::Login(_Out std::string & name, _Out std::string & password)
{
	SetFormSize(60, 8);
	//显示光标
	SetCursorVisible(true);

	//需要画出来的string
	string str_title("Login");
	string str_name("Name:");
	string str_password("Password:");
	string str_none;
	str_none.resize(sizeof(char) * 32, ' ');
	unsigned length_name = str_name.length();
	unsigned length_password = str_password.length();
	unsigned length_none = str_none.length();

	//清屏，画界面
	system("cls");
	
	COORD coord;
	coord.X = (m_FormSizeX - str_title.length()) / 2;
	coord.Y = 1;
	SetConsoleCursorPosition(m_Handle, coord);
	SetConsoleTextAttribute(m_Handle, 03);
	cout << str_title;

	//用户输入的开始
InputBegin:
	//考虑到从输入密码切换到输入账户的时候重置输入框空白处，直接重画这部分
	coord.X = (m_FormSizeX - length_password - length_none) / 2 + length_password - length_name;
	coord.Y = 3;
	SetConsoleCursorPosition(m_Handle, coord);
	SetConsoleTextAttribute(m_Handle, 03);
	cout << str_name;
	SetConsoleTextAttribute(m_Handle, 0xF7);
	cout << str_none;

	coord.X = (m_FormSizeX - length_password - length_none) / 2;
	coord.Y = 5;
	SetConsoleCursorPosition(m_Handle, coord);
	SetConsoleTextAttribute(m_Handle, 03);
	cout << str_password;
	SetConsoleTextAttribute(m_Handle, 0xF7);
	cout << str_none;

	char key;
	SetConsoleTextAttribute(m_Handle, 0xF0);
	//移动光标到name后面
	coord.X = (m_FormSizeX - length_password - length_none) / 2 + length_password;
	coord.Y = 3;
	SetConsoleCursorPosition(m_Handle, coord);
	name.clear();
	if (Input(0xf0, 0x0f, length_none, name) == ControlStatus::Before)
	{
		return ControlStatus::JumpOut;
	}

	//移动光标到password后面
	coord.X = (m_FormSizeX - length_password - length_none) / 2 + length_password;
	coord.Y = 5;
	SetConsoleCursorPosition(m_Handle, coord);
	password.clear();
	if (Input(0xf0, 0x0f, length_none, password, '*') == ControlStatus::Before)
	{
		goto InputBegin;
	}
	return ControlStatus::SuccessOut;
}

CR::ControlStatus CR::Client_UI::Register(_Out std::string & name, _Out std::string & password, _Out string & invitationCode)
{
	SetFormSize(60, 12);
	//显示光标
	SetCursorVisible(true);

	//需要画出来的string
	string str_title("Register");
	string str_name("Name:");
	string str_password("Password:");
	string str_code("Code");
	string str_again("Again");
	string str_none;
	str_none.resize(sizeof(char) * 32, ' ');
	unsigned length_name = str_name.length();
	unsigned length_password = str_password.length();
	unsigned length_none = str_none.length();
	unsigned length_code = str_code.length();

	//清屏，画界面
	SetConsoleTextAttribute(m_Handle, 0x0f);
	system("cls");

	COORD coord;
	coord.X = (m_FormSizeX - str_title.length()) / 2;
	coord.Y = 1;
	SetConsoleCursorPosition(m_Handle, coord);
	SetConsoleTextAttribute(m_Handle, 03);
	cout << str_title;

	//用户输入的开始
ImportBegin:
	//考虑到从输入密码切换到输入账户的时候重置输入框空白处，直接重画这部分
	coord.X = (m_FormSizeX - length_password - length_none) / 2 + length_password - length_name;
	coord.Y = 3;
	SetConsoleCursorPosition(m_Handle, coord);
	SetConsoleTextAttribute(m_Handle, 03);
	cout << str_name;
	SetConsoleTextAttribute(m_Handle, 0xF7);
	cout << str_none;
	SetConsoleCursorPosition(m_Handle, coord);

	coord.X = (m_FormSizeX - length_password - length_none) / 2;
	coord.Y = 5;
	SetConsoleCursorPosition(m_Handle, coord);
	SetConsoleTextAttribute(m_Handle, 03);
	cout << str_password;
	SetConsoleTextAttribute(m_Handle, 0xF7);
	cout << str_none;

	coord.X = (m_FormSizeX - length_password - length_none) / 2;
	coord.Y = 7;
	SetConsoleCursorPosition(m_Handle, coord);
	SetConsoleTextAttribute(m_Handle, 03);
	cout << str_password;
	SetConsoleTextAttribute(m_Handle, 0xF7);
	cout << str_none;
	SetConsoleTextAttribute(m_Handle, 03);
	cout << str_again;

	coord.X = (m_FormSizeX - length_password - length_none) / 2 + length_password - length_code;
	coord.Y = 9;
	SetConsoleCursorPosition(m_Handle, coord);
	SetConsoleTextAttribute(m_Handle, 03);
	cout << str_code;
	SetConsoleTextAttribute(m_Handle, 0xF7);
	cout << str_none;

	int controlCatch = 0;
	const int inputPlaceX = (m_FormSizeX - length_password - length_none) / 2 + length_password;
	while (1)
	{
		switch (controlCatch)
		{
		case 0:
		{
			name.clear();
			coord.X = inputPlaceX;
			coord.Y = 3;
			//填充空白
			SetConsoleCursorPosition(m_Handle, coord);
			SetConsoleTextAttribute(m_Handle, 0xF7);
			cout << str_none;
			//用户输入
			SetConsoleCursorPosition(m_Handle, coord);
			if (Input(0xf0, 0x0f, length_none, name) == ControlStatus::Before)
			{
				return ControlStatus::JumpOut;
			}
			else
			{
				controlCatch++;
			}
		}break;
		case 1:
		{
			password.clear();
			coord.X = inputPlaceX;
			coord.Y = 5;
			//填充空白
			SetConsoleCursorPosition(m_Handle, coord);
			SetConsoleTextAttribute(m_Handle, 0xF7);
			cout << str_none;
			//用户输入
			SetConsoleCursorPosition(m_Handle, coord);
			if (Input(0xf0, 0x0f, length_none, password, '*') == ControlStatus::Before)
			{
				controlCatch--;
			}
			else
			{
				controlCatch++;
			}
		}break;
		case 2:
		{
			coord.X = inputPlaceX;
			coord.Y = 7;
			//填充空白
			SetConsoleCursorPosition(m_Handle, coord);
			SetConsoleTextAttribute(m_Handle, 0xF7);
			cout << str_none;
			//用户输入
			SetConsoleCursorPosition(m_Handle, coord);
			string passAgain;
			if (Input(0xf0, 0x0f, length_none, passAgain, '*') == ControlStatus::Before)
			{
				controlCatch--;
			}
			else //如果第二次输入密码的时候确认了，先检查两次密码是否相等
			{
				if (passAgain == password)								//密码正确跳到下一个控件
				{
					controlCatch++;
				}
				else																			//密码错误提示
				{
					SetConsoleCursorPosition(m_Handle, coord);
					SetConsoleTextAttribute(m_Handle, 0xFc);
					cout << "**Two passwords are different!**";
					//隐藏光标
					SetCursorVisible(false);
					_getch();
					//恢复光标
					SetCursorVisible(true);
				}
			}
		}break;
		case 3:
		{
			invitationCode.clear();
			coord.X = inputPlaceX;
			coord.Y = 9;
			//填充空白
			SetConsoleCursorPosition(m_Handle, coord);
			SetConsoleTextAttribute(m_Handle, 0xF7);
			cout << str_none;
			//用户输入
			SetConsoleCursorPosition(m_Handle, coord);
			if (Input(0xf0, 0x0f, length_none, invitationCode) == ControlStatus::Before)
			{
				controlCatch--;
			}
			else
			{
				controlCatch++;
			}
		}break;
		default:
		{
			return ControlStatus::SuccessOut;							//输入完成
		}break;
		}
	}
	return ControlStatus::WrongOut;
}

UINT32 CR::Client_UI::ShowFolder(_In std::vector<FileMsg> & fMsg, _In const std::string & rootString)
{
	unsigned page = 0, size = fMsg.size(), maxPage = size / 7 + (bool)(size % 7);
	while (1)
	{
		DrawFolder(fMsg, page, rootString);

		//画完之后进入输入控制阶段
		char key;
		while(1)
		{
			key = _getch();
			if (key >= '1' && key <= '7')							//1~7是用来下载文件的按钮
			{
				UINT32 sub = page * 7 + key - '1';
				if (sub <= size)											//如果下标存在 ，返回下标
				{
					return sub;
				}
			}
			else if (key == '8')											//上一页
			{
				if (page)
				{
					page--;
				}
			}
			else if (key == '9')											//下一页
			{
				if (page < maxPage)
				{
					page++;
				}
			}
			else if (key == '0')
			{
				return -1;
			}
			else
			{
				continue;
			}
			break;
		}
	}
}

bool CR::Client_UI::ConfirmDownLoad(_In FileMsg & fmsg)
{
	system("cls");
	SetCursorVisible(false);
	unsigned fs_X = 80, fs_Y = 7;
	COORD cur_Pos;
	cur_Pos.X = 78;
	cur_Pos.Y = 1;
	SetFormSize(fs_X, fs_Y);
	SetConsoleTextAttribute(m_Handle, 0x0f);
	//绘制确认界面
	printf("┌");
	for (int drawTimes = 1; drawTimes < 78; drawTimes++)
	{
		printf("─");
	}
	printf("┐");
	putchar('\n');
	for (int drawTimes = fs_Y - 2; drawTimes > 0; drawTimes--)
	{
		printf("│");
		SetConsoleCursorPosition(m_Handle, cur_Pos);
		printf("│\n");
		cur_Pos.Y++;
	}
	printf("└");
	for (int drawTimes = 1; drawTimes < 78; drawTimes++)
	{
		printf("─");
	}
	printf("┘");
	cur_Pos.X = 32, cur_Pos.Y = 1;
	SetConsoleCursorPosition(m_Handle, cur_Pos);
	SetConsoleTextAttribute(m_Handle, 0x04);
	printf("Confirm DownLoad");
	cur_Pos.X = 2, cur_Pos.Y++;
	SetConsoleTextAttribute(m_Handle, 0x0f);
	SetConsoleCursorPosition(m_Handle, cur_Pos);
	printf("Name:  ");
	cout << fmsg.name;
	cur_Pos.Y++;
	SetConsoleCursorPosition(m_Handle, cur_Pos);
	printf("Size:  %uBytes", fmsg.size);
	cur_Pos.Y+=2;
	SetConsoleCursorPosition(m_Handle, cur_Pos);
	SetConsoleTextAttribute(m_Handle, 0x03);
	printf("Enter");
	SetConsoleTextAttribute(m_Handle, 0x0f);
	printf(":Confirm        ");
	SetConsoleTextAttribute(m_Handle, 0x03);
	printf("Esc");
	SetConsoleTextAttribute(m_Handle, 0x0f);
	printf(":Back");
	//输入检测
	char key;
	while (1)
	{
		key = _getch();
		switch (key)
		{
		case '\r'://Enter
		{
			return true;
		}
		case 27://Esc
		{
			return false;
		}
			break;
		default:
			break;
		}
	}
}

void CR::Client_UI::DownLoading_1(_In FileMsg & fmsg)
{
	system("cls");
	SetCursorVisible(false);
	unsigned fs_X = 80, fs_Y = 7;
	COORD cur_Pos;
	cur_Pos.X = 78;
	cur_Pos.Y = 1;
	SetFormSize(fs_X, fs_Y);
	SetConsoleTextAttribute(m_Handle, 0x0f);
	//绘制确认界面
	printf("┌");
	for (int drawTimes = 1; drawTimes < 78; drawTimes++)
	{
		printf("─");
	}
	printf("┐");
	putchar('\n');
	for (int drawTimes = fs_Y - 2; drawTimes > 0; drawTimes--)
	{
		printf("│");
		SetConsoleCursorPosition(m_Handle, cur_Pos);
		printf("│\n");
		cur_Pos.Y++;
	}
	printf("└");
	for (int drawTimes = 1; drawTimes < 78; drawTimes++)
	{
		printf("─");
	}
	printf("┘");
	cur_Pos.X = 35, cur_Pos.Y = 1;
	SetConsoleCursorPosition(m_Handle, cur_Pos);
	SetConsoleTextAttribute(m_Handle, 0x04);
	printf("DownLoading");
	cur_Pos.Y++, cur_Pos.X = 3;
	SetConsoleTextAttribute(m_Handle, 0x0f);
	SetConsoleCursorPosition(m_Handle, cur_Pos);
	cout << fmsg.name;
	cur_Pos.Y++;
	cur_Pos.X += 15;
	SetConsoleCursorPosition(m_Handle, cur_Pos);
	printf("/%10uBytes", fmsg.size);
	DownLoading_2(fmsg, 0);																			//重置
}

bool CR::Client_UI::DownLoading_2(_In FileMsg & fmsg, _In unsigned addSize)
{
	static UINT64 size;
	if (addSize)
	{
		size += addSize;
	}
	else
	{
		size = 0;
	}
	COORD cur_Pos;
	cur_Pos.X = 3;
	cur_Pos.Y = 3;
	SetConsoleCursorPosition(m_Handle, cur_Pos);
	SetConsoleTextAttribute(m_Handle, 0x03);
	printf("%10u", size);
	SetConsoleTextAttribute(m_Handle, 0x0f);
	printf("Bytes");
	cur_Pos.X += 31;
	SetConsoleCursorPosition(m_Handle, cur_Pos);
	putchar('(');
	SetConsoleTextAttribute(m_Handle, 0x03);
	printf("%.2f", size * 1.0f / fmsg.size * 100.0f);
	SetConsoleTextAttribute(m_Handle, 0x0f);
	printf("%%)");
	if (size == fmsg.size)																							//判断下载是否完成
	{
		system("cls");
		cur_Pos.X = 31,	cur_Pos.Y = 1;
		SetConsoleTextAttribute(m_Handle, 0x04);
		SetConsoleCursorPosition(m_Handle, cur_Pos);
		printf("DownLoad Success!");
		cur_Pos.Y+=2, cur_Pos.X = 37;
		SetConsoleCursorPosition(m_Handle, cur_Pos);
		SetConsoleTextAttribute(m_Handle, 0x03);
		printf("Enter");
		while (_getch() != '\r');
		return true;
	}
	else
	{
		return false;
	}
}

CR::ControlStatus CR::Client_UI::Input(_In int wordColor, _In int backColor, _In int maxLength, _Out std::string & str, _In char out)
{
	SetConsoleTextAttribute(m_Handle, wordColor);
	char key;
	while (1)							//输入循环
	{
		key = _getch();			//捕抓键盘输入
		if (str.length() < maxLength && key >= 'A'&&key <= 'z' || key == '_' || key >= '0' && key <= '9')				//正常密码输入
		{
			if (out)
			{
				putchar(out);
			}
			else
			{
				putchar(key);
			}
			str.push_back(key);
		}
		else if (key == '\b' && str.length())																									//退格键
		{
			putchar('\b');
			SetConsoleTextAttribute(m_Handle, wordColor);
			putchar(' ');
			putchar('\b');
			str.pop_back();
		}
		else if (key == '\r' && str.length())																		//输入完成
		{
			return ControlStatus::Next;
		}
		else if (key == 27)																									//27是ESC，表示取消输入
		{
			return ControlStatus::Before;
		}
	}
}

inline void CR::Client_UI::DrawFolder(_In std::vector<FileMsg> & fMsg, _In unsigned page, _In const std::string & rootString)
{
	SetFormSize(80, 14);

	COORD curPos;
	curPos.X = curPos.Y = 0;
	SetConsoleTextAttribute(m_Handle, 0x0F);														//黑底白字画边框
	system("cls");
	SetConsoleCursorPosition(m_Handle, curPos);													//指针归位
	//隐藏光标																		
	SetCursorVisible(false);
	//非数据部分
	printf("┌──┬");
	for (int drawTimes = 4; drawTimes < 78; drawTimes++)
	{
		printf("─");
	}
	printf("┐");
	curPos.Y++;
	SetConsoleCursorPosition(m_Handle, curPos);
	printf("│  │");
	curPos.X += 78;
	SetConsoleCursorPosition(m_Handle, curPos);
	printf("│\n");
	curPos.Y++;
	curPos.X = 0;
	SetConsoleCursorPosition(m_Handle, curPos);
	printf("├──┼");
	for (int drawTimes = 4; drawTimes < 78; drawTimes++)
	{
		printf("─");
	}
	printf("┤\n");
	curPos.Y++;
	curPos.X = 78;
	for (int drawTimes = 1; drawTimes < 11; drawTimes++)
	{
		printf("│ %d│", drawTimes % 10);
		SetConsoleCursorPosition(m_Handle, curPos);
		printf("│\n");
		curPos.Y++;
	}
	printf("└──┴");
	for (int drawTimes = 4; drawTimes < 78; drawTimes++)
	{
		printf("─");
	}
	printf("┘");
	SetConsoleTextAttribute(m_Handle, 0x03);															//设置三个特殊按键的颜色,湖蓝色字体
	curPos.X = 5;
	curPos.Y = 10;
	SetConsoleCursorPosition(m_Handle, curPos);
	printf("↑Page Up!");
	curPos.Y++;
	SetConsoleCursorPosition(m_Handle, curPos);
	printf("↓Page Down!");
	curPos.Y++;
	SetConsoleCursorPosition(m_Handle, curPos);
	printf("←Previous Folder!");
	
	curPos.Y = 1;
	SetConsoleCursorPosition(m_Handle, curPos);
	SetConsoleTextAttribute(m_Handle, 0x05);
	printf("Root:  ");
	cout << rootString;

	//数据部分														
	curPos.Y = 3;
	curPos.X = 5;
	SetConsoleCursorPosition(m_Handle, curPos);
	int upSub = page * 7;																							//第一个文件的下标
	int leftFiles = fMsg.size() - upSub;
	int nFile = leftFiles > 7 ? 7 : leftFiles;																//判断该分页有多少个文件
	for (int fileThisPage = 0; fileThisPage < nFile; fileThisPage++)				//输出文件名
	{
		int sub = upSub + fileThisPage;
		SetConsoleTextAttribute(m_Handle, (fMsg[sub].type == FileType::File) ? 0x0F : 0x06);//黑底白字文件，黑底黄字文件夹
		cout << fMsg[sub].name;
		curPos.Y++;
		SetConsoleCursorPosition(m_Handle, curPos);
	}
}

void CR::Client_UI::SetCursorVisible(bool isVisible)
{
	CONSOLE_CURSOR_INFO cci;
	GetConsoleCursorInfo(m_Handle, &cci);
	cci.bVisible = isVisible;
	SetConsoleCursorInfo(m_Handle, &cci);
}
