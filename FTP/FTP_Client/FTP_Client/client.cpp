#include "client.h"

CR::Client_Transport::Client_Transport() :m_RecvN(0), m_IsWaiting(true), m_pIsFileRecv(nullptr), m_IsWorking(false)
{
	//��ȡ����Ŀ¼
	char buffer[MAX_PATH]{};
	_getcwd(buffer, MAX_PATH);
	m_Path = buffer;
	//��ʼ��Winsock
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))			//���ط����ʾʧ��
	{
		MessageBox(NULL, "Winsock��ʼ��ʧ�ܣ�", "����", MB_OK);
		exit(EXIT_FAILURE);
	}
	ZeroMemory(m_Buffer, 2048 * sizeof(char));
	//���÷�������ַ��Ϣ
	m_ServAddr.sin_family = AF_INET;                                  //ipv4ģʽ
	m_ServAddr.sin_port = htons(4567);								  //�����˿�
	//����socketģʽ
	m_Socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

CR::Client_Transport::~Client_Transport()
{
	m_IsWorking = false;
	m_RecvTh.join();
	if (m_pIsFileRecv)
	{
		delete[] m_pIsFileRecv;
	}
}

bool CR::Client_Transport::Connect(_In std::string & ip)
{
	m_ServIpv4 = ip;
	UINT32 ipAddr = 0;
	inet_pton(AF_INET, ip.c_str(), &ipAddr);
	m_ServAddr.sin_addr.S_un.S_addr = ipAddr;																													//ע��ip��ַ
	if (::connect(m_Socket, (sockaddr *)&m_ServAddr, sizeof(m_ServAddr)) == INVALID_SOCKET)						//��������
	{
		MessageBox(nullptr, "����ʧ�ܣ�", "����", MB_OK);
		//PostQuitMessage(0);
		return false;
	}
	//���������߳�
	m_IsWorking = true;
	m_RecvTh = std::thread(&Client_Transport::Recevid, this);
	return true;
}

bool CR::Client_Transport::Login(_In std::string & name, _In std::string & password)
{
	assert(m_ServIpv4.length());																			//����Connect֮��m_ServIpv4���г���
	char buffer[256]{};																								//������
	INT32 type = static_cast<INT32>(RecvType::Login);
	memcpy_s(buffer, 4, &type, 4);																		//���õ�¼ģʽ																						
	memcpy_s(buffer + 4, 32, name.c_str(), name.length());							//�����û���
	memcpy_s(buffer + 36, 32, password.c_str(), password.length());		//��������
	::send(m_Socket, buffer, _Login_Length, 0);
	if (WaitTime(30000))
	{
		switch (static_cast<LoginType>(BufferProcess()))
		{
		case CR::LoginType::Success:
		{
			return true;
		}
			break;
		case CR::LoginType::WrongPassword:
		{
			MessageBox(nullptr, "�������", "��¼ʧ��", MB_OK);
			return false;
		}
			break;
		case CR::LoginType::NoneUser:
		{
			MessageBox(nullptr, "�û������ڣ�", "��¼ʧ��", MB_OK);
			return false;
		}
			break;
		default:
		{

		}
			break;
		}
	}
	return false;
}

void CR::Client_Transport::Register(_In std::string & name, _In std::string & password, _In std::string invitationCode)
{
	char buffer[2048]{};
	INT32 type = static_cast<INT32>(RecvType::Register);
	memcpy_s(buffer, 4, &type, 4);																		//ע�봫��ģʽ
	memcpy_s(buffer + 4, 32, name.c_str(), name.length());							//ע���û���
	memcpy_s(buffer + 36, 32, password.c_str(), password.length());		//ע������
	memcpy_s(buffer + 68, 16, invitationCode.c_str(), invitationCode.length());//ע��������
	::send(m_Socket, buffer, 2048, 0);
	if (WaitTime(10000))
	{
		switch (static_cast<RegisterType>(BufferProcess()))
		{
		case RegisterType::Success:
		{
			MessageBox(nullptr, "�������ص�¼ǰ���档", "ע��ɹ�", MB_OK);
		}
			break;
		case RegisterType::InvCodeUsed:
		{
			MessageBox(nullptr, "��������Ч��ʹ�ã�", "ע��ʧ��", MB_OK);
		}
			break;
		case RegisterType::UserExisted:
		{
			MessageBox(nullptr, "�û��Ѵ��ڣ�", "ע��ʧ��", MB_OK);
		}
			break;
		default:
			break;
		}
	}
}

const std::vector<CR::FileMsg>& CR::Client_Transport::GetDepotMessage()
{
	char buffer[260]{};
	INT32 type = static_cast<INT32>(RecvType::List);
	memcpy_s(buffer, 4, &type, 4);
	memcpy_s(buffer + 4, 4, m_RootM.c_str(), m_RootM.length());
	m_FileList.clear();
	::send(m_Socket, buffer, 260, 0);
	while (1)													//�ȴ�Ŀ¼��ȡ����
	{
		WaitTime(2000);							//ʱ�䲻��Ҫ
		if (!BufferProcess() && m_LastPackType == RecvType::List)
		{
			break;
		}
	}
	return m_FileList;
}

void CR::Client_Transport::UpdateDepot(_In INT32 sub)
{
	if (sub == -1)				//�����ϲ�Ŀ¼
	{
		if (m_RootM.empty())
		{
			return;
		}
		int lsub = m_RootM.length() - 1;
		while (m_RootM[lsub] != '/')
		{
			lsub--;
		}
		m_RootM.erase(m_RootM.begin() + lsub, m_RootM.end());//MayBug
	}
	else
	{
		assert(m_FileList[sub].type == FileType::Folder);
		m_RootM += "/";
		m_RootM += m_FileList[sub].name;																				//��Ŀ¼����������ļ���
	}
}

bool CR::Client_Transport::DownLoadStart(_In INT32 sub)
{
	m_FileMsg = m_FileList[sub];
	assert(m_FileMsg.type == FileType::File);
	INT32 type = static_cast<INT32>(RecvType::File_Begin);
	string name = m_RootM + m_FileMsg.name;
	INT32 length = name.length();
	char tbuffer[276]{};
	memcpy_s(tbuffer, 4, &type, 4);						//����ģʽ
	memcpy_s(tbuffer + 4, 4, &length, 4);			//д�����Ƴ���
	memcpy_s(tbuffer + 8, 8, &m_FileMsg.size, 8);		//д���ļ���С
	memcpy_s(tbuffer + 16, length, name.c_str(), length);//д���ļ���
	::send(m_Socket, tbuffer, 276, 0);
	WaitTime(2000);
	if (BufferProcess() > 0)
	{
		assert(m_LastPackType == RecvType::File_Begin);
		return true;
	}
	return false;
}

INT32 CR::Client_Transport::DownLoading()
{
	WaitTime(2000);
	INT32 res = BufferProcess();
	assert(m_LastPackType == RecvType::File_Keep);
	return res;
}

const std::string & CR::Client_Transport::GetRootMString()
{
	return m_RootM;
}

void CR::Client_Transport::Recevid()
{
	while (m_IsWorking)
	{
		if (m_IsWaiting)
		{
			m_RecvN = ::recv(m_Socket, m_Buffer, 2048, MSG_WAITALL);
			Sleep(1);
			if (m_RecvN > 0)
			{
				SetIsWaiting(false);
			}
		}
	}
}

bool CR::Client_Transport::WaitTime(DWORD time)
{
	DWORD deTime = timeGetTime() + time;
	while (deTime > timeGetTime())
	{
		if (!m_IsWaiting)												//�����ͣ������Ϣ��֤����Ϣ����
		{
			return true;
		}
	}
	return false;
}

INT32 CR::Client_Transport::BufferProcess()
{
	INT32 cty;
	memcpy_s(&cty, 4, m_Buffer, 4);
	m_LastPackType = static_cast<RecvType>(cty);
	switch (m_LastPackType)
	{
	case CR::RecvType::Close:
	{

	}
		break;
	case CR::RecvType::Login:
	{
		INT32 ret;
		memcpy_s(&ret, 4, m_Buffer + 4, 4);
		SetIsWaiting(true);
		return ret;
	}
		break;

	case CR::RecvType::Register:
	{
		INT32 ret;
		memcpy_s(&ret, 4, m_Buffer + 4, 4);
		SetIsWaiting(true);
		return ret;
	}
		break;

	case CR::RecvType::Keep:
	{
		INT32 ret;
		memcpy_s(&ret, 4, m_Buffer + 4, 4);
		SetIsWaiting(true);
		if (ret == -1)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
		break;

	case CR::RecvType::List:																				//�ڷ���ֵΪ0֮ǰ�ϲ㲻����������
	{
		INT32 laveList;																							//�Ƿ�Ҫ����
		memcpy_s(&laveList, 4, m_Buffer + 4, 4);
		INT32 nFiles;																								//��δ�����ļ�����
		memcpy_s(&nFiles, 4, m_Buffer + 8, 4);
		char * movesize = m_Buffer + 12;															//ָ���ƶ�����
		UINT32 fnLength;																						//�ļ�����С
		UINT64 fsize;																								//�ļ���С
		INT32 ftype;																									//�ļ�����
		char tBuffer[512]{};
		for (nFiles; nFiles > 0; nFiles--)																//ѭ����ȡ�ļ��б�
		{
			memcpy_s(&fnLength, 4, movesize, 4);						//��ȡ�ļ�������
			movesize += 4;
			memcpy_s(&fsize, 8, movesize, 8);								//��ȡ�ļ���С
			movesize += 8;
			memcpy_s(&ftype, 4, movesize, 4);								//��ȡ�ļ����� 
			movesize += 4;
			ZeroMemory(tBuffer, 512);
			memcpy_s(tBuffer, 512, movesize, fnLength);			//��ȡ�ļ���
			m_FileList.push_back(FileMsg(tBuffer, static_cast<FileType>(ftype), fsize));								//��ӵ��ļ��б�
			movesize += fnLength;
		}
		SetIsWaiting(true);
		return laveList;
	}
		break;

	case CR::RecvType::File_Begin:
	{
		INT32 result;
		memcpy_s(&result, 4, m_Buffer + 4, 4);										//�ļ�����������
		switch (static_cast<FileAskType>(result))
		{
		case FileAskType::Different:																		//OK�Ļ����طְ���Ŀ
		{

		}
			break;
		case FileAskType::None:
		{

		}
			break;
		default:																									//���򷵻�������Ϣ
		{
			memcpy_s(&m_AllPacks, 4, m_Buffer + 4, 4);
			m_pIsFileRecv = new bool[m_AllPacks + 1]{};
			m_File.open(m_Path + "\\DownLoad\\" + m_FileMsg.name, std::ios_base::out | std::ios::binary);//ֻдģʽ����
			m_LastPackSub = 0;																		//�������
			m_LePacks = m_AllPacks;
			SetIsWaiting(true);
			return m_AllPacks;
		}
			break;
		}
	}
		break;

	case CR::RecvType::File_Keep:
	{
		INT32 pack, size;
		memcpy_s(&pack, 4, m_Buffer + 4, 4);//��ð��ı��
		//if (m_pIsFileRecv[pack])					//�ظ���
		//{
		//	SetIsWaiting(true);
		//	return 0;
		//}
		memcpy_s(&size, 4, m_Buffer + 8, 4);//����������ݴ�С
		//if (pack - m_LastPackSub > 1)				  //��˳�����
		//{
		//	INT32 largePack;
		//	for (largePack = m_AllPacks; largePack > 0; largePack--)//�ȼ�鵱ǰд���С
		//	{
		//		if (m_pIsFileRecv[largePack])
		//		{
		//			break;
		//		}
		//	}
		//	if (largePack > pack)//�Ѿ�д���
		//	{
		//		m_File.seekp(size * (pack - 1), std::ios_base::beg);//��λ���ð�Ӧ�ô����λ��
		//		m_File.write(m_Buffer + 2048 - size, size);
		//	}
		//	else								//û��д���
		//	{
		//		m_File.seekp(0, std::ios_base::end);							//��λ���ļ�β
		//		for (int i = pack - largePack; i > 0; i--)
		//		{
		//			m_File.write(m_Buffer + 2048 - size, size);
		//		}
		//	}
		//}
		//else if (pack - m_LastPackSub < 1)		//��˳�����
		//{
		//	m_File.seekp(size * (pack - 1), std::ios_base::beg);//��λ���ð�Ӧ�ô����λ��
		//	m_File.write(m_Buffer + 2048 - size, size);
		//}
		//else
		//{
		m_File.write(m_Buffer + 12, size);
		//}
		m_LePacks--;										//ʣ���--
		if (!m_LePacks)									//����Ƿ�������,������Ͼ͹ر��ļ���
		{
			delete [] m_pIsFileRecv;
			m_pIsFileRecv = nullptr;
			m_File.close();
		}
		else                                                        //���û�н�����ϣ����ղŽ��ܵ��ǲ������һ����
		{
			m_pIsFileRecv[pack] = true;	 //����Ѿ��յ��ð�
			m_LastPackSub = pack;
			if (pack == m_AllPacks)
			{
				//UNDO:�����ܵ����һ��������ǰ�滹��û���յİ�
				char buffer[2048]{};
				memcpy_s(buffer, 4, &m_LastPackType, 4);
				memcpy_s(buffer + 4, 2044, m_pIsFileRecv, m_AllPacks + 1);
				::send(m_Socket, buffer, 2048, 0);
			}
		}
		SetIsWaiting(true);
		return size;
	}
		break;

	case CR::RecvType::File_Last:
	{

	}
		break;

	default:
	{

	}
		break;
	}
	SetIsWaiting(true);
	return 0;
}

void CR::Client_Transport::SetIsWaiting(bool is)
{
	m_IsWaiting = is;
}
