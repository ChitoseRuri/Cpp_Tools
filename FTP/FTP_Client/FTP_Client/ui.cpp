#include "ui.h"

CR::Client_UI::Client_UI():m_Handle(GetStdHandle(STD_OUTPUT_HANDLE))
{
	//���ô������
	SetConsoleTitle(CONSOLE_TITLE);
	//�Ƴ���󻯰�ť
	DeleteMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_MAXIMIZE, MF_BYCOMMAND);
	//���ô��ڴ�С
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
	//���ƽ���
	SetConsoleTextAttribute(m_Handle, 0x0f);
	COORD curPos;
	curPos.Y = 4;
	curPos.X = 22;
	SetConsoleCursorPosition(m_Handle, curPos);
	SetConsoleTextAttribute(m_Handle, 0xf3);
	SetConsoleCursorPosition(m_Handle, curPos);
	//��ȡ�ļ��鿴��һ�εķ�������ַ
	std::fstream recFile;
	recFile.open("Data/ip.dat");
	string str[4];
	unsigned ip_s[4];
	char key;																	//�����������
	bool isFileOK = true;
	int strCount;
	//��Ᵽ���ļ��Ƿ���
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
	//ȷ�ϼ����
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
	//�ȴ�����
	bool isInput = true;
	while (isInput)
	{
		key = _getch();
		switch (key)
		{
		case '\b':
		{
			if (str[strCount].empty())					//������ַ����ǿյ�
			{
				if (strCount)										//�Ҳ������һ���ַ���
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
	//��ַ�ֶ����ϵ�serverIP
	for (int add_t = 0; add_t < 4; add_t++)
	{
		serverIP += str[add_t];
		serverIP.push_back('.');
	}
	serverIP.pop_back();
	//����ε�ip��ַ��¼
	recFile << serverIP;
	recFile.close();
	return ControlStatus::SuccessOut;
}

CR::MenuList CR::Client_UI::Main()
{
	SetFormSize(60, 8);
	//���ع��
	SetCursorVisible(false);
	SetConsoleTextAttribute(m_Handle, 0x03);
	system("cls");
	//��Ҫ��������string
	string str_title("Welcome to the Deport~");
	string str_login("Login");
	string str_register("Register");

	unsigned length_title = str_title.length();
	unsigned length_login = str_login.length();
	unsigned length_register = str_register.length();

	//��ɫ����
	int color[2]{ 0x03,0xf3 };											//�ڵ����֡��׵�����

	//������������
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

		//��ץ�����¼�
		char key = _getch();
		if (key == 9)																																	//����tab��
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
	//��ʾ���
	SetCursorVisible(true);

	//��Ҫ��������string
	string str_title("Login");
	string str_name("Name:");
	string str_password("Password:");
	string str_none;
	str_none.resize(sizeof(char) * 32, ' ');
	unsigned length_name = str_name.length();
	unsigned length_password = str_password.length();
	unsigned length_none = str_none.length();

	//������������
	system("cls");
	
	COORD coord;
	coord.X = (m_FormSizeX - str_title.length()) / 2;
	coord.Y = 1;
	SetConsoleCursorPosition(m_Handle, coord);
	SetConsoleTextAttribute(m_Handle, 03);
	cout << str_title;

	//�û�����Ŀ�ʼ
InputBegin:
	//���ǵ������������л��������˻���ʱ�����������հ״���ֱ���ػ��ⲿ��
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
	//�ƶ���굽name����
	coord.X = (m_FormSizeX - length_password - length_none) / 2 + length_password;
	coord.Y = 3;
	SetConsoleCursorPosition(m_Handle, coord);
	name.clear();
	if (Input(0xf0, 0x0f, length_none, name) == ControlStatus::Before)
	{
		return ControlStatus::JumpOut;
	}

	//�ƶ���굽password����
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
	//��ʾ���
	SetCursorVisible(true);

	//��Ҫ��������string
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

	//������������
	SetConsoleTextAttribute(m_Handle, 0x0f);
	system("cls");

	COORD coord;
	coord.X = (m_FormSizeX - str_title.length()) / 2;
	coord.Y = 1;
	SetConsoleCursorPosition(m_Handle, coord);
	SetConsoleTextAttribute(m_Handle, 03);
	cout << str_title;

	//�û�����Ŀ�ʼ
ImportBegin:
	//���ǵ������������л��������˻���ʱ�����������հ״���ֱ���ػ��ⲿ��
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
			//���հ�
			SetConsoleCursorPosition(m_Handle, coord);
			SetConsoleTextAttribute(m_Handle, 0xF7);
			cout << str_none;
			//�û�����
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
			//���հ�
			SetConsoleCursorPosition(m_Handle, coord);
			SetConsoleTextAttribute(m_Handle, 0xF7);
			cout << str_none;
			//�û�����
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
			//���հ�
			SetConsoleCursorPosition(m_Handle, coord);
			SetConsoleTextAttribute(m_Handle, 0xF7);
			cout << str_none;
			//�û�����
			SetConsoleCursorPosition(m_Handle, coord);
			string passAgain;
			if (Input(0xf0, 0x0f, length_none, passAgain, '*') == ControlStatus::Before)
			{
				controlCatch--;
			}
			else //����ڶ������������ʱ��ȷ���ˣ��ȼ�����������Ƿ����
			{
				if (passAgain == password)								//������ȷ������һ���ؼ�
				{
					controlCatch++;
				}
				else																			//���������ʾ
				{
					SetConsoleCursorPosition(m_Handle, coord);
					SetConsoleTextAttribute(m_Handle, 0xFc);
					cout << "**Two passwords are different!**";
					//���ع��
					SetCursorVisible(false);
					_getch();
					//�ָ����
					SetCursorVisible(true);
				}
			}
		}break;
		case 3:
		{
			invitationCode.clear();
			coord.X = inputPlaceX;
			coord.Y = 9;
			//���հ�
			SetConsoleCursorPosition(m_Handle, coord);
			SetConsoleTextAttribute(m_Handle, 0xF7);
			cout << str_none;
			//�û�����
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
			return ControlStatus::SuccessOut;							//�������
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

		//����֮�����������ƽ׶�
		char key;
		while(1)
		{
			key = _getch();
			if (key >= '1' && key <= '7')							//1~7�����������ļ��İ�ť
			{
				UINT32 sub = page * 7 + key - '1';
				if (sub <= size)											//����±���� �������±�
				{
					return sub;
				}
			}
			else if (key == '8')											//��һҳ
			{
				if (page)
				{
					page--;
				}
			}
			else if (key == '9')											//��һҳ
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
	//����ȷ�Ͻ���
	printf("��");
	for (int drawTimes = 1; drawTimes < 78; drawTimes++)
	{
		printf("��");
	}
	printf("��");
	putchar('\n');
	for (int drawTimes = fs_Y - 2; drawTimes > 0; drawTimes--)
	{
		printf("��");
		SetConsoleCursorPosition(m_Handle, cur_Pos);
		printf("��\n");
		cur_Pos.Y++;
	}
	printf("��");
	for (int drawTimes = 1; drawTimes < 78; drawTimes++)
	{
		printf("��");
	}
	printf("��");
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
	//������
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
	//����ȷ�Ͻ���
	printf("��");
	for (int drawTimes = 1; drawTimes < 78; drawTimes++)
	{
		printf("��");
	}
	printf("��");
	putchar('\n');
	for (int drawTimes = fs_Y - 2; drawTimes > 0; drawTimes--)
	{
		printf("��");
		SetConsoleCursorPosition(m_Handle, cur_Pos);
		printf("��\n");
		cur_Pos.Y++;
	}
	printf("��");
	for (int drawTimes = 1; drawTimes < 78; drawTimes++)
	{
		printf("��");
	}
	printf("��");
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
	DownLoading_2(fmsg, 0);																			//����
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
	if (size == fmsg.size)																							//�ж������Ƿ����
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
	while (1)							//����ѭ��
	{
		key = _getch();			//��ץ��������
		if (str.length() < maxLength && key >= 'A'&&key <= 'z' || key == '_' || key >= '0' && key <= '9')				//������������
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
		else if (key == '\b' && str.length())																									//�˸��
		{
			putchar('\b');
			SetConsoleTextAttribute(m_Handle, wordColor);
			putchar(' ');
			putchar('\b');
			str.pop_back();
		}
		else if (key == '\r' && str.length())																		//�������
		{
			return ControlStatus::Next;
		}
		else if (key == 27)																									//27��ESC����ʾȡ������
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
	SetConsoleTextAttribute(m_Handle, 0x0F);														//�ڵװ��ֻ��߿�
	system("cls");
	SetConsoleCursorPosition(m_Handle, curPos);													//ָ���λ
	//���ع��																		
	SetCursorVisible(false);
	//�����ݲ���
	printf("��������");
	for (int drawTimes = 4; drawTimes < 78; drawTimes++)
	{
		printf("��");
	}
	printf("��");
	curPos.Y++;
	SetConsoleCursorPosition(m_Handle, curPos);
	printf("��  ��");
	curPos.X += 78;
	SetConsoleCursorPosition(m_Handle, curPos);
	printf("��\n");
	curPos.Y++;
	curPos.X = 0;
	SetConsoleCursorPosition(m_Handle, curPos);
	printf("��������");
	for (int drawTimes = 4; drawTimes < 78; drawTimes++)
	{
		printf("��");
	}
	printf("��\n");
	curPos.Y++;
	curPos.X = 78;
	for (int drawTimes = 1; drawTimes < 11; drawTimes++)
	{
		printf("�� %d��", drawTimes % 10);
		SetConsoleCursorPosition(m_Handle, curPos);
		printf("��\n");
		curPos.Y++;
	}
	printf("��������");
	for (int drawTimes = 4; drawTimes < 78; drawTimes++)
	{
		printf("��");
	}
	printf("��");
	SetConsoleTextAttribute(m_Handle, 0x03);															//�����������ⰴ������ɫ,����ɫ����
	curPos.X = 5;
	curPos.Y = 10;
	SetConsoleCursorPosition(m_Handle, curPos);
	printf("��Page Up!");
	curPos.Y++;
	SetConsoleCursorPosition(m_Handle, curPos);
	printf("��Page Down!");
	curPos.Y++;
	SetConsoleCursorPosition(m_Handle, curPos);
	printf("��Previous Folder!");
	
	curPos.Y = 1;
	SetConsoleCursorPosition(m_Handle, curPos);
	SetConsoleTextAttribute(m_Handle, 0x05);
	printf("Root:  ");
	cout << rootString;

	//���ݲ���														
	curPos.Y = 3;
	curPos.X = 5;
	SetConsoleCursorPosition(m_Handle, curPos);
	int upSub = page * 7;																							//��һ���ļ����±�
	int leftFiles = fMsg.size() - upSub;
	int nFile = leftFiles > 7 ? 7 : leftFiles;																//�жϸ÷�ҳ�ж��ٸ��ļ�
	for (int fileThisPage = 0; fileThisPage < nFile; fileThisPage++)				//����ļ���
	{
		int sub = upSub + fileThisPage;
		SetConsoleTextAttribute(m_Handle, (fMsg[sub].type == FileType::File) ? 0x0F : 0x06);//�ڵװ����ļ����ڵ׻����ļ���
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
