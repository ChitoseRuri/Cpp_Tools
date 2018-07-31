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
		if (client.Connect(ip))															//���ӷ������ɹ�
		{
			while (1)
			{

			MainMenu:																			//���˵�
				switch (ui.Main())
				{
				case CR::MenuList::Login:
				{
					string name, password;
					if (ui.Login(name, password) == CR::ControlStatus::SuccessOut)							//�������
					{
						if (client.Login(name, password))																				//��¼�ɹ�
						{
							while (1)
							{
								m_FileVector = client.GetDepotMessage();															//��ȡĿ¼�ļ��б�
								INT32 sub = ui.ShowFolder(m_FileVector, client.GetRootMString());			//չʾ
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
										while (ui.DownLoading_2(m_FileVector[sub], client.DownLoading()));	//�������ع��̲���ʾ
									}
								}
							}
						}
					}
					else																															 			  //ȡ�����룬�ص����˵�
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