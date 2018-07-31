#include "server.h"

CR::Server_ForeClient::Server_ForeClient(SOCKET & csocket, sockaddr & csaddr, bool * isuse, bool * isFileUsing) :
	m_Socket(csocket), m_Saddr(csaddr), m_pIsUse(isuse), m_IsLoginSuccess(0), m_IsFileUsing(isFileUsing),
	m_RecvThread(std::thread(&Server_ForeClient::Recevid, this)), m_IsWorking(true), m_IsWaiting(true)
{
	char buffer[MAX_PATH]{};
	_getcwd(buffer, MAX_PATH);
	m_Path = buffer;
	m_Path += "/Data";
	*m_pIsUse = true;
	MainDeal();
}

CR::Server_ForeClient::~Server_ForeClient()
{
	m_IsWorking = false;
	m_RecvThread.join();//�ȴ��̹߳ر�
	*m_pIsUse = false;
}

void CR::Server_ForeClient::MainDeal()
{
	while (1)
	{
		if (WaitTime(60000))
		{
			BufferProcess();
			m_IsWaiting = true;
		}
		else
		{
			Server_ForeClient::~Server_ForeClient();
		}
	}
}

void CR::Server_ForeClient::Recevid()
{
	while (m_IsWorking)
	{
		if (m_IsWaiting)
		{
			m_RecvN = ::recv(m_Socket, m_Buffer, 1024, 0);
			if (m_RecvN > 0)
			{
				m_IsWaiting = false;
			}
		}
		else
		{

		}
	}
}

bool CR::Server_ForeClient::WaitTime(DWORD time)
{
	DWORD deTime = timeGetTime() + time;
	while (deTime > timeGetTime())
	{
		if (!m_IsWaiting)												//�����ͣ������Ϣ��֤������Ϣû�б�����
		{
			return true;
		}
	}
	return false;
}

int CR::Server_ForeClient::BufferProcess()
{
	INT32 cty;
	memcpy_s(&cty, 4, m_Buffer, 4);
	printf("%d ", cty);
	m_LastPackType = static_cast<RecvType>(cty);
	switch (m_LastPackType)
	{
	case CR::RecvType::Close:
	{
		Server_ForeClient::~Server_ForeClient();
	}
		break;
	case CR::RecvType::Login:
	{
		char name[32]{}, password[32]{};
		memcpy_s(name, 32, m_Buffer + 4, 32);			//��ȡ�û���
		memcpy_s(password, 32, m_Buffer + 36, 32);//��ȡ�û�����
		INT32 res = static_cast<INT32>(Login(name, password));
		if (res == static_cast<INT32>(LoginType::Success))
		{
			m_IsLoginSuccess = 1;
		}
		char tempBuffer[2048]{};
		memcpy_s(tempBuffer, 4, &cty, 4);
		memcpy_s(tempBuffer + 4, 4, &res, 4);
		::send(m_Socket, tempBuffer, 2048, 0);
		return res;
	}
		break;
	case CR::RecvType::Register:
	{
		char name[32]{}, password[32]{}, invCode[16]{};
		memcpy_s(name, 32, m_Buffer + 4, 32);						//��ȡ�û���
		memcpy_s(password, 32, m_Buffer + 36, 32);			//��ȡ�û�����
		memcpy_s(invCode, 16, m_Buffer + 68, 16);				//��ȡ������
		INT32 res = static_cast<INT32>(Register(name, password, invCode));
		char tempBuffer[2048];
		memcpy_s(tempBuffer, 4, &cty, 4);
		memcpy_s(tempBuffer + 4, 4, &res, 4);
		::send(m_Socket, tempBuffer, 2048, 0);
		return res;
	}
		break;
	case CR::RecvType::Keep:
	{
		m_IsLoginSuccess++;
		return 1;
	}
		break;
	case CR::RecvType::List:
	{
		FileList();
		return 1;
	}
		break;
	case CR::RecvType::File_Begin:
	{
		string filePath;
		FileAskType result = FileBegin(filePath);
		INT32 tbuffer = static_cast<INT32>(result);	
		switch (result)
		{
		case CR::FileAskType::OK:															//��ʼѭ�������ļ�
		{
			SendFile(filePath);
		}
			break;
		default:
		{
			::send(m_Socket, (char*)(&tbuffer), 4, 0);							//���������ֱ�ӷ���������
		}
			break;
		}
		return tbuffer;
	}
		break;
	case CR::RecvType::File_Keep:
		break;
	case CR::RecvType::File_Last:
		break;
	default:
		break;
	}
}

void CR::Server_ForeClient::IsOnlineCheck()
{
	while (m_IsWorking)
	{
		Sleep(10000);
		m_IsLoginSuccess--;
		if (m_IsLoginSuccess < 0)												//���߱��С��0����ʾ�ͻ��˳�ʱ��û�з���������Ϣ
		{
			break;
		}
	}
	Server_ForeClient::~Server_ForeClient();						//�رո��߳�
}

CR::LoginType CR::Server_ForeClient::Login(_In std::string name, _In std::string password)
{
	while (m_IsFileUsing[0]);													//�ȴ��ļ����⿪
	m_IsFileUsing[0] = true;
	std::fstream file("Server/Login.dat");
	string fName, fPassword;
	while (!file.eof())
	{
		file >> fName >> fPassword;
		if (fName == name)
		{
			file.close();
			m_IsFileUsing[0] = false;
			if (fPassword == password)
			{
				return LoginType::Success;
			}
			else
			{
				return LoginType::WrongPassword;
			}
		}
	}
	file.close();
	m_IsFileUsing[0] = false;
	return LoginType::NoneUser;
}

CR::RegisterType CR::Server_ForeClient::Register(_In std::string name, _In std::string password, _In std::string invitationCode)
{
	while (m_IsFileUsing[1]);													//�ȴ��ļ����⿪
	m_IsFileUsing[1] = true;
	std::fstream invcFile("Server/InvCode.dat"); 
	string invc, strn, strp;
	bool isUsed;
	//Ѱ�Ҹü������Ƿ�ʹ��
	while (!invcFile.eof())
	{
		invcFile >> invc >> isUsed;
		if (invc == invitationCode)
		{
			if (isUsed)
			{
				invcFile.close();
				m_IsFileUsing[1] = false;
				return RegisterType::InvCodeUsed;
			}
			else
			{
				invcFile.seekg(-1, std::ios::cur);//�������ļ���ָ��ָ��ü������ʹ�ñ��
				break;
			}
		}
	}
	while (m_IsFileUsing[0]);
	m_IsFileUsing[0] = true;
	std::fstream loginFile("Server/Login.dat", std::ios::in | std::ios::out);
	assert(loginFile.is_open());
	//Ѱ�Ҹ��û����Ƿ�ע��
	while (!loginFile.eof())
	{
		loginFile >> strn >> strp;
		if (strn == name)
		{
			invcFile.close();
			loginFile.close();
			m_IsFileUsing[0] = m_IsFileUsing[1] = false;
			return RegisterType::UserExisted;
		}
	}
	invcFile << "1";														//ʹ�øü�����
	invcFile.close();
	loginFile.seekg(0, std::ios::end);				//ע���дָ��ָ��β��
	loginFile << std::endl << name << " " << password;	//�û���������д��ע���б�				
	loginFile.close();
	m_IsFileUsing[0] = m_IsFileUsing[1] = false;
	return RegisterType::Success;
}

void CR::Server_ForeClient::FileList()
{
	//�Ȼ�ȡ�������ļ����·��
	char tempBuffer[2048]{};
	memcpy_s(tempBuffer, 260, m_Buffer + 4, 260);
	string path = m_Path + tempBuffer;
	std::vector<FileMsg> fileNames;
	//����·����ʼ��ȡ�ļ���
	GetAllFiles(path, fileNames);
	fileNames.erase(fileNames.begin(), fileNames.begin() + 2);
	unsigned size = fileNames.size();
	unsigned bSub = 0, eSub = 0, length = 12;
	INT32 mode = static_cast<INT32>(RecvType::List); //�����ʽ
	INT32 more = 0;																	 //�Ƿ�Ҫ��������ı�־
	INT32 nNames = 0;												    		 //���δ�����ļ�������
	INT32 ftype;																			 //�ļ�����
	UINT32 nlength;																	 //�ļ�������
	ZeroMemory(tempBuffer, 2048);
	memcpy_s(tempBuffer, 4, &mode, 4);														//д�봫��ģʽ
	char * desBuffer = tempBuffer + 12;															//������ָ��
	while (eSub < size)
	{
		length += 16;
		length += nlength = fileNames[eSub].name.length();
		if (length < 2048)																						//����û��
		{
			nNames++;																								//������++			
			memcpy_s(desBuffer, 4, &nlength, 4);												//��������ļ����ĳ���
			desBuffer += 4;
			memcpy_s(desBuffer, 8, &fileNames[eSub].size, 8);						//��������ļ�����
			desBuffer += 8;
			ftype = static_cast<INT32>(fileNames[eSub].type);
			memcpy_s(desBuffer, 4, &ftype, 4);													//�����ļ�����
			desBuffer += 4;
			memcpy_s(desBuffer, length, fileNames[eSub].name.c_str(), length);//�����ļ���
			desBuffer += nlength;
			eSub++;
		}
		else																													//�������������������
		{
			more = 1;																									//��Ҫ��������
			memcpy_s(tempBuffer + 4, 4, &more, 4);											//д���ж��Ƿ�Ҫ��������
			memcpy_s(tempBuffer + 8, 4, &nNames, 4);									//д�뱾�δ����ļ�������
			::send(m_Socket, tempBuffer, 2048, 0);
			bSub = eSub;																							//����ͷ�����
			length = 12;																								//���û���������
			desBuffer = tempBuffer + 12;																//������ָ��
			nNames = 0;																								//����������
		}
	}
	//�������һ���ļ���û����2048����
	more = 0;
	memcpy_s(tempBuffer + 4, 4, &more, 4);													//д���ж��Ƿ�Ҫ��������
	memcpy_s(tempBuffer + 8, 4, &nNames, 4);											//д�뱾�δ����ļ�������
	::send(m_Socket, tempBuffer, 2048, 0);												
}

CR::FileAskType CR::Server_ForeClient::FileBegin(_Out std::string & filePath)
{
	INT32 nameLength;
	INT64 fileSize;
	memcpy_s(&nameLength, 4, m_Buffer + 4, 4);										//��ȡ�ļ�������
	memcpy_s(&fileSize, 8, m_Buffer + 8, 8);												//��ȡ�ļ���С
	char name[MAX_PATH]{};																			//�ļ���������·��
	memcpy_s(&name, nameLength, m_Buffer + 16, nameLength);		//��ȡ�ļ���
	filePath = m_Path + "\\" + name;
	std::fstream file(filePath);
	if (!file.is_open())																							//����ļ���ʧ��
	{
		return FileAskType::None;
	}
	file.seekg(0, std::ios_base::end);
	INT64 fSize = file.tellg();
	file.close();
	if (fileSize != fSize)
	{
		return FileAskType::Different;
	}
	return FileAskType::OK;
}

bool CR::Server_ForeClient::SendFile(_In std::string & filePath)
{
	std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::binary);
	bool check = file.is_open();
	char tempBuffer[2048]{};
	//��ȡ�ļ���С
	file.seekg(0, std::ios_base::end);
	INT64 fSize = file.tellg();
	
	 check = file.is_open();
	//���ö�ָ��
	file.seekg(0, std::ios_base::beg);
	INT32 forePack = 2048 - 4 - 4 - 4;													//ÿ��Я�����ļ���Ϣ��С
	INT32 times = fSize / forePack + (bool)(fSize%forePack);		//������Ҫ����Ĵ���
	INT32 type = static_cast<INT32>(RecvType::File_Begin);
	memcpy_s(tempBuffer, 4, &type, 4);				
	memcpy_s(tempBuffer + 4, 4, &times, 4);
	::send(m_Socket, tempBuffer, 2048, 0);							//���ͷְ����
	type = static_cast<INT32>(RecvType::File_Keep); //�����������Լ��ְ���Ŀ
	memcpy_s(tempBuffer, 4, &type, 4);							  //���ô���ģʽ
	int i;
	for (i = 1; i < times; i++)												//�������һ������Ĵ���
	{
		memcpy_s(tempBuffer + 4, 4, &i, 4);						//д��ְ����
		memcpy_s(tempBuffer + 8, 4, &forePack, 4);		//д��ð���Ч��Ϣ��С
		file.read(tempBuffer + 12, forePack);						//���ļ��ж�ȡ����
		::send(m_Socket, tempBuffer, 2048, 0);				//����
	}
	memcpy_s(tempBuffer + 4, 4, &i, 4);
	INT32 lastPack = fSize % forePack;							//���һ���Ĵ�С
	memcpy_s(tempBuffer + 8, 4, &lastPack, 4);
	file.read(tempBuffer + 12, lastPack);							//���һ�δ�������
	::send(m_Socket, tempBuffer, 2048, 0);					//����
	return true;
}

CR::Server_Terminal::Server_Terminal()
{
	//�ļ�����ʼ��
	memset(m_FileUsing, 0, sizeof(m_FileUsing));
	//��ʼ��Winsock
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))			//���ط����ʾʧ��
	{
		MessageBox(NULL, "Winsock��ʼ��ʧ�ܣ�", "����", MB_OK);
		exit(EXIT_FAILURE);
	}
	m_ListenSock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(4567);
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	if (::bind(m_ListenSock, (sockaddr*)&saddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		MessageBoxA(NULL, "��ʧ��", "����", MB_OK);
		exit(EXIT_FAILURE);
	}
	if (::listen(m_ListenSock, 20) == SOCKET_ERROR)
	{
		MessageBoxA(NULL, "����ʧ��", "����", MB_OK);
		exit(EXIT_FAILURE);
	}
}

CR::Server_Terminal::~Server_Terminal()
{
	for (Server & server : m_Clients)
	{
		delete server.pData;
	}
}

void CR::Server_Terminal::Work()
{
	sockaddr newSaddr;
	SOCKET newSocket = -1;
	int size = sizeof(sockaddr);
	while (1)
	{
		while (1)
		{
			newSocket = ::accept(m_ListenSock, &newSaddr, &size);
			if (newSocket == -1)
			{
				//Sleep(233);
			}
			else
			{
				break;
			}
		}
		int size = m_Clients.size();
		bool isnCreated = true;
		for (int i = 0; i < size; i++)	//��������������û���Ѿ��Ͽ����ӵĶ���
		{
			if (!m_Clients[i].isUse)//�еĻ�ֱ�����������¹���
			{
				delete m_Clients[i].pData;
				m_Clients[i].pData = new Server_ForeClient(newSocket, newSaddr, &m_Clients[i].isUse, m_FileUsing);
				isnCreated = false;
				break;
			}
		}
		if (isnCreated)//û�еĻ��Ÿ��µĽ�ȥ
		{
			m_Clients.push_back(Server());
			m_Clients[size].pData = new Server_ForeClient(newSocket, newSaddr, &m_Clients[size].isUse, m_FileUsing);
		}
	}
}

void CR::GetAllFiles(_In std::string path, _Out std::vector<FileMsg> & names)
{
	INT64 hFile = 0;
	//�ļ���Ϣ      
	struct _finddata_t fileinfo;//�����洢�ļ���Ϣ�Ľṹ��      
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)  //��һ�β���    
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))  //������ҵ������ļ���    
			{
				names.push_back(FileMsg(p.assign(fileinfo.name), FileType::Folder, 0));
			}
			else //������ҵ��Ĳ������ļ���     
			{
				names.push_back(FileMsg(p.assign(fileinfo.name), FileType::File, fileinfo.size));
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile); //��������    
	}
}