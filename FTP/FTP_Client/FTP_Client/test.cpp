#include "test.h"

void CR_Test::Test_UI_Login()
{
	CR::Client_UI ui;
	std::string name, password;
	ui.Login(name, password);
	system("cls");
	std::cout << "Client_UI::Login£º" << std::endl << "	name:" << name << std::endl << "	password:" << password;
}

void CR_Test::Test_UI_Main()
{
	CR::Client_UI ui;
	ui.Main();
}

void CR_Test::Test_UI_DrawFolder()
{
	CR::Client_UI ui;
	std::vector<CR::FileMsg> nfv;
	ui.ShowFolder(nfv,"asfas");
}

void CR_Test::Test_UI_Connect()
{
	CR::Client_UI ui;
	std::string str;
	ui.ConnectMenu(str);
}

void CR_Test::Test_UI_ConfirmDownLoad()
{
	CR::Client_UI ui;
	CR::FileMsg k("Bokuno hero academy", 232312388);
	ui.ConfirmDownLoad(k);
}

void CR_Test::Test_UI_DownLoading()
{
	CR::Client_UI ui;
	CR::FileMsg k("Bokuno hero academy_12.mp4", 40813224562);
	ui.DownLoading_1(k);
	ui.DownLoading_2(k, 0);
	ui.DownLoading_2(k, 1005002154);
}
