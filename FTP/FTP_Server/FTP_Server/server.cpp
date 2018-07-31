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
	m_RecvThread.join();//等待线程关闭
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
		if (!m_IsWaiting)												//如果暂停接收信息，证明有信息没有被处理
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
		memcpy_s(name, 32, m_Buffer + 4, 32);			//获取用户名
		memcpy_s(password, 32, m_Buffer + 36, 32);//获取用户密码
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
		memcpy_s(name, 32, m_Buffer + 4, 32);						//获取用户名
		memcpy_s(password, 32, m_Buffer + 36, 32);			//获取用户密码
		memcpy_s(invCode, 16, m_Buffer + 68, 16);				//获取邀请码
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
		case CR::FileAskType::OK:															//开始循环发送文件
		{
			SendFile(filePath);
		}
			break;
		default:
		{
			::send(m_Socket, (char*)(&tbuffer), 4, 0);							//非正常情况直接返回请求结果
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
		if (m_IsLoginSuccess < 0)												//在线标记小于0，表示客户端长时间没有发送在线信息
		{
			break;
		}
	}
	Server_ForeClient::~Server_ForeClient();						//关闭该线程
}

CR::LoginType CR::Server_ForeClient::Login(_In std::string name, _In std::string password)
{
	while (m_IsFileUsing[0]);													//等待文件锁解开
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
	while (m_IsFileUsing[1]);													//等待文件锁解开
	m_IsFileUsing[1] = true;
	std::fstream invcFile("Server/InvCode.dat"); 
	string invc, strn, strp;
	bool isUsed;
	//寻找该激活码是否被使用
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
				invcFile.seekg(-1, std::ios::cur);//激活码文件流指针指向该激活码的使用标记
				break;
			}
		}
	}
	while (m_IsFileUsing[0]);
	m_IsFileUsing[0] = true;
	std::fstream loginFile("Server/Login.dat", std::ios::in | std::ios::out);
	assert(loginFile.is_open());
	//寻找该用户名是否被注册
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
	invcFile << "1";														//使用该激活码
	invcFile.close();
	loginFile.seekg(0, std::ios::end);				//注册表写指针指向尾部
	loginFile << std::endl << name << " " << password;	//用户名和密码写入注册列表				
	loginFile.close();
	m_IsFileUsing[0] = m_IsFileUsing[1] = false;
	return RegisterType::Success;
}

void CR::Server_ForeClient::FileList()
{
	//先获取传来的文件相对路径
	char tempBuffer[2048]{};
	memcpy_s(tempBuffer, 260, m_Buffer + 4, 260);
	string path = m_Path + tempBuffer;
	std::vector<FileMsg> fileNames;
	//传入路径开始获取文件名
	GetAllFiles(path, fileNames);
	fileNames.erase(fileNames.begin(), fileNames.begin() + 2);
	unsigned size = fileNames.size();
	unsigned bSub = 0, eSub = 0, length = 12;
	INT32 mode = static_cast<INT32>(RecvType::List); //传输格式
	INT32 more = 0;																	 //是否还要继续传输的标志
	INT32 nNames = 0;												    		 //本次传输的文件名个数
	INT32 ftype;																			 //文件类型
	UINT32 nlength;																	 //文件名长度
	ZeroMemory(tempBuffer, 2048);
	memcpy_s(tempBuffer, 4, &mode, 4);														//写入传输模式
	char * desBuffer = tempBuffer + 12;															//数据流指针
	while (eSub < size)
	{
		length += 16;
		length += nlength = fileNames[eSub].name.length();
		if (length < 2048)																						//缓存没满
		{
			nNames++;																								//计数器++			
			memcpy_s(desBuffer, 4, &nlength, 4);												//传入这个文件名的长度
			desBuffer += 4;
			memcpy_s(desBuffer, 8, &fileNames[eSub].size, 8);						//传入这个文件长度
			desBuffer += 8;
			ftype = static_cast<INT32>(fileNames[eSub].type);
			memcpy_s(desBuffer, 4, &ftype, 4);													//传入文件类型
			desBuffer += 4;
			memcpy_s(desBuffer, length, fileNames[eSub].name.c_str(), length);//传入文件名
			desBuffer += nlength;
			eSub++;
		}
		else																													//如果加上这个缓存就满了
		{
			more = 1;																									//还要继续传输
			memcpy_s(tempBuffer + 4, 4, &more, 4);											//写入判断是否要继续传输
			memcpy_s(tempBuffer + 8, 4, &nNames, 4);									//写入本次传输文件名个数
			::send(m_Socket, tempBuffer, 2048, 0);
			bSub = eSub;																							//重置头部标号
			length = 12;																								//重置缓存区长度
			desBuffer = tempBuffer + 12;																//重置流指针
			nNames = 0;																								//计数器重置
		}
	}
	//到了最后一个文件还没超过2048长度
	more = 0;
	memcpy_s(tempBuffer + 4, 4, &more, 4);													//写入判断是否要继续传输
	memcpy_s(tempBuffer + 8, 4, &nNames, 4);											//写入本次传输文件名个数
	::send(m_Socket, tempBuffer, 2048, 0);												
}

CR::FileAskType CR::Server_ForeClient::FileBegin(_Out std::string & filePath)
{
	INT32 nameLength;
	INT64 fileSize;
	memcpy_s(&nameLength, 4, m_Buffer + 4, 4);										//获取文件名长度
	memcpy_s(&fileSize, 8, m_Buffer + 8, 8);												//获取文件大小
	char name[MAX_PATH]{};																			//文件名，包含路径
	memcpy_s(&name, nameLength, m_Buffer + 16, nameLength);		//获取文件名
	filePath = m_Path + "\\" + name;
	std::fstream file(filePath);
	if (!file.is_open())																							//如果文件打开失败
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
	//获取文件大小
	file.seekg(0, std::ios_base::end);
	INT64 fSize = file.tellg();
	
	 check = file.is_open();
	//重置读指针
	file.seekg(0, std::ios_base::beg);
	INT32 forePack = 2048 - 4 - 4 - 4;													//每次携带的文件信息大小
	INT32 times = fSize / forePack + (bool)(fSize%forePack);		//计算需要传输的次数
	INT32 type = static_cast<INT32>(RecvType::File_Begin);
	memcpy_s(tempBuffer, 4, &type, 4);				
	memcpy_s(tempBuffer + 4, 4, &times, 4);
	::send(m_Socket, tempBuffer, 2048, 0);							//发送分包结果
	type = static_cast<INT32>(RecvType::File_Keep); //发送请求结果以及分包数目
	memcpy_s(tempBuffer, 4, &type, 4);							  //设置传输模式
	int i;
	for (i = 1; i < times; i++)												//除了最后一次以外的传输
	{
		memcpy_s(tempBuffer + 4, 4, &i, 4);						//写入分包序号
		memcpy_s(tempBuffer + 8, 4, &forePack, 4);		//写入该包有效信息大小
		file.read(tempBuffer + 12, forePack);						//从文件中读取数据
		::send(m_Socket, tempBuffer, 2048, 0);				//发送
	}
	memcpy_s(tempBuffer + 4, 4, &i, 4);
	INT32 lastPack = fSize % forePack;							//最后一包的大小
	memcpy_s(tempBuffer + 8, 4, &lastPack, 4);
	file.read(tempBuffer + 12, lastPack);							//最后一次传入数据
	::send(m_Socket, tempBuffer, 2048, 0);					//发送
	return true;
}

CR::Server_Terminal::Server_Terminal()
{
	//文件锁初始化
	memset(m_FileUsing, 0, sizeof(m_FileUsing));
	//初始化Winsock
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))			//返回非零表示失败
	{
		MessageBox(NULL, "Winsock初始化失败！", "错误", MB_OK);
		exit(EXIT_FAILURE);
	}
	m_ListenSock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(4567);
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	if (::bind(m_ListenSock, (sockaddr*)&saddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		MessageBoxA(NULL, "绑定失败", "错误", MB_OK);
		exit(EXIT_FAILURE);
	}
	if (::listen(m_ListenSock, 20) == SOCKET_ERROR)
	{
		MessageBoxA(NULL, "监听失败", "错误", MB_OK);
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
		for (int i = 0; i < size; i++)	//搜索容器里面有没有已经断开连接的对象
		{
			if (!m_Clients[i].isUse)//有的话直接在里面重新构造
			{
				delete m_Clients[i].pData;
				m_Clients[i].pData = new Server_ForeClient(newSocket, newSaddr, &m_Clients[i].isUse, m_FileUsing);
				isnCreated = false;
				break;
			}
		}
		if (isnCreated)//没有的话放个新的进去
		{
			m_Clients.push_back(Server());
			m_Clients[size].pData = new Server_ForeClient(newSocket, newSaddr, &m_Clients[size].isUse, m_FileUsing);
		}
	}
}

void CR::GetAllFiles(_In std::string path, _Out std::vector<FileMsg> & names)
{
	INT64 hFile = 0;
	//文件信息      
	struct _finddata_t fileinfo;//用来存储文件信息的结构体      
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)  //第一次查找    
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))  //如果查找到的是文件夹    
			{
				names.push_back(FileMsg(p.assign(fileinfo.name), FileType::Folder, 0));
			}
			else //如果查找到的不是是文件夹     
			{
				names.push_back(FileMsg(p.assign(fileinfo.name), FileType::File, fileinfo.size));
			}
		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile); //结束查找    
	}
}