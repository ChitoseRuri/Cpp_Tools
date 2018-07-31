#include "main.h"

using std::string;

int main()
{
	if (1)
	{
		CR::Client_UI ui;
		CR::Client_Transport client;
		std::vector<CR::FileMsg>m_FileVector;
		string ip;
		ui.ConnectMenu(ip);
		if (client.Connect(ip))															//连接服务器成功
		{
			while (1)
			{

			MainMenu:																			//主菜单
				switch (ui.Main())
				{
				case CR::MenuList::Login:
				{
					string name, password;
					if (ui.Login(name, password) == CR::ControlStatus::SuccessOut)							//输入完成
					{
						if (client.Login(name, password))																				//登录成功
						{
							while (1)
							{
								m_FileVector = client.GetDepotMessage();															//获取目录文件列表
								INT32 sub = ui.ShowFolder(m_FileVector, client.GetRootMString());			//展示
								if (sub == -1 || m_FileVector[sub].type == CR::FileType::Folder)
								{
									client.UpdateDepot(sub);
								}
								else
								{
									if (ui.ConfirmDownLoad(m_FileVector[sub]))
									{
										ui.DownLoading_1(m_FileVector[sub]);
										client.DownLoadStart(sub);
										while (ui.DownLoading_2(m_FileVector[sub], client.DownLoading()));	//处理下载过程并显示
									}
								}
							}
						}
					}
					else																															 			  //取消输入，回到主菜单
					{
						goto MainMenu;
					}
				}
				break;
				case CR::MenuList::Register:
				{
					string name, password, invitationCode;
					ui.Register(name, password, invitationCode);
					client.Register(name, password, invitationCode);
				}
				break;
				}
			}
		}
	}
	else
	{
		CR_Test::Test_UI_DownLoading();
		_getch();
	}
	return 0;
}