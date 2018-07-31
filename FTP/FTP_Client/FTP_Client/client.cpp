#include "client.h"

CR::Client_Transport::Client_Transport() :m_RecvN(0), m_IsWaiting(true), m_pIsFileRecv(nullptr), m_IsWorking(false)
{
	//获取运行目录
	char buffer[MAX_PATH]{};
	_getcwd(buffer, MAX_PATH);
	m_Path = buffer;
	//初始化Winsock
	WSADATA wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))			//返回非零表示失败
	{
		MessageBox(NULL, "Winsock初始化失败！", "错误", MB_OK);
		exit(EXIT_FAILURE);
	}
	ZeroMemory(m_Buffer, 2048 * sizeof(char));
	//设置服务器地址信息
	m_ServAddr.sin_family = AF_INET;                                  //ipv4模式
	m_ServAddr.sin_port = htons(4567);								  //监听端口
	//设置socket模式
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
	m_ServAddr.sin_addr.S_un.S_addr = ipAddr;																													//注入ip地址
	if (::connect(m_Socket, (sockaddr *)&m_ServAddr, sizeof(m_ServAddr)) == INVALID_SOCKET)						//请求连接
	{
		MessageBox(nullptr, "连接失败！", "错误", MB_OK);
		//PostQuitMessage(0);
		return false;
	}
	//创建接收线程
	m_IsWorking = true;
	m_RecvTh = std::thread(&Client_Transport::Recevid, this);
	return true;
}

bool CR::Client_Transport::Login(_In std::string & name, _In std::string & password)
{
	assert(m_ServIpv4.length());																			//经过Connect之后m_ServIpv4才有长度
	char buffer[256]{};																								//缓冲区
	INT32 type = static_cast<INT32>(RecvType::Login);
	memcpy_s(buffer, 4, &type, 4);																		//设置登录模式																						
	memcpy_s(buffer + 4, 32, name.c_str(), name.length());							//输入用户名
	memcpy_s(buffer + 36, 32, password.c_str(), password.length());		//输入密码
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
			MessageBox(nullptr, "密码错误！", "登录失败", MB_OK);
			return false;
		}
			break;
		case CR::LoginType::NoneUser:
		{
			MessageBox(nullptr, "用户不存在！", "登录失败", MB_OK);
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
	memcpy_s(buffer, 4, &type, 4);																		//注入传输模式
	memcpy_s(buffer + 4, 32, name.c_str(), name.length());							//注入用户名
	memcpy_s(buffer + 36, 32, password.c_str(), password.length());		//注入密码
	memcpy_s(buffer + 68, 16, invitationCode.c_str(), invitationCode.length());//注入邀请码
	::send(m_Socket, buffer, 2048, 0);
	if (WaitTime(10000))
	{
		switch (static_cast<RegisterType>(BufferProcess()))
		{
		case RegisterType::Success:
		{
			MessageBox(nullptr, "即将返回登录前界面。", "注册成功", MB_OK);
		}
			break;
		case RegisterType::InvCodeUsed:
		{
			MessageBox(nullptr, "邀请码无效或被使用！", "注册失败", MB_OK);
		}
			break;
		case RegisterType::UserExisted:
		{
			MessageBox(nullptr, "用户已存在！", "注册失败", MB_OK);
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
	while (1)													//等待目录获取结束
	{
		WaitTime(2000);							//时间不重要
		if (!BufferProcess() && m_LastPackType == RecvType::List)
		{
			break;
		}
	}
	return m_FileList;
}

void CR::Client_Transport::UpdateDepot(_In INT32 sub)
{
	if (sub == -1)				//返回上层目录
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
		m_RootM += m_FileList[sub].name;																				//根目录延续添加新文件夹
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
	memcpy_s(tbuffer, 4, &type, 4);						//设置模式
	memcpy_s(tbuffer + 4, 4, &length, 4);			//写入名称长度
	memcpy_s(tbuffer + 8, 8, &m_FileMsg.size, 8);		//写入文件大小
	memcpy_s(tbuffer + 16, length, name.c_str(), length);//写入文件名
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
		if (!m_IsWaiting)												//如果暂停接收信息，证明信息到了
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

	case CR::RecvType::List:																				//在返回值为0之前上层不做其它操作
	{
		INT32 laveList;																							//是否还要传输
		memcpy_s(&laveList, 4, m_Buffer + 4, 4);
		INT32 nFiles;																								//这次传输的文件个数
		memcpy_s(&nFiles, 4, m_Buffer + 8, 4);
		char * movesize = m_Buffer + 12;															//指针移动距离
		UINT32 fnLength;																						//文件名大小
		UINT64 fsize;																								//文件大小
		INT32 ftype;																									//文件类型
		char tBuffer[512]{};
		for (nFiles; nFiles > 0; nFiles--)																//循环获取文件列表
		{
			memcpy_s(&fnLength, 4, movesize, 4);						//获取文件名长度
			movesize += 4;
			memcpy_s(&fsize, 8, movesize, 8);								//获取文件大小
			movesize += 8;
			memcpy_s(&ftype, 4, movesize, 4);								//获取文件类型 
			movesize += 4;
			ZeroMemory(tBuffer, 512);
			memcpy_s(tBuffer, 512, movesize, fnLength);			//获取文件名
			m_FileList.push_back(FileMsg(tBuffer, static_cast<FileType>(ftype), fsize));								//添加到文件列表
			movesize += fnLength;
		}
		SetIsWaiting(true);
		return laveList;
	}
		break;

	case CR::RecvType::File_Begin:
	{
		INT32 result;
		memcpy_s(&result, 4, m_Buffer + 4, 4);										//文件下载请求结果
		switch (static_cast<FileAskType>(result))
		{
		case FileAskType::Different:																		//OK的话返回分包数目
		{

		}
			break;
		case FileAskType::None:
		{

		}
			break;
		default:																									//否则返回类型信息
		{
			memcpy_s(&m_AllPacks, 4, m_Buffer + 4, 4);
			m_pIsFileRecv = new bool[m_AllPacks + 1]{};
			m_File.open(m_Path + "\\DownLoad\\" + m_FileMsg.name, std::ios_base::out | std::ios::binary);//只写模式创建
			m_LastPackSub = 0;																		//重置序号
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
		memcpy_s(&pack, 4, m_Buffer + 4, 4);//获得包的标号
		//if (m_pIsFileRecv[pack])					//重复包
		//{
		//	SetIsWaiting(true);
		//	return 0;
		//}
		memcpy_s(&size, 4, m_Buffer + 8, 4);//这个包的数据大小
		//if (pack - m_LastPackSub > 1)				  //过顺序操作
		//{
		//	INT32 largePack;
		//	for (largePack = m_AllPacks; largePack > 0; largePack--)//先检查当前写入大小
		//	{
		//		if (m_pIsFileRecv[largePack])
		//		{
		//			break;
		//		}
		//	}
		//	if (largePack > pack)//已经写入过
		//	{
		//		m_File.seekp(size * (pack - 1), std::ios_base::beg);//定位到该包应该储存的位置
		//		m_File.write(m_Buffer + 2048 - size, size);
		//	}
		//	else								//没有写入过
		//	{
		//		m_File.seekp(0, std::ios_base::end);							//定位到文件尾
		//		for (int i = pack - largePack; i > 0; i--)
		//		{
		//			m_File.write(m_Buffer + 2048 - size, size);
		//		}
		//	}
		//}
		//else if (pack - m_LastPackSub < 1)		//负顺序操作
		//{
		//	m_File.seekp(size * (pack - 1), std::ios_base::beg);//定位到该包应该储存的位置
		//	m_File.write(m_Buffer + 2048 - size, size);
		//}
		//else
		//{
		m_File.write(m_Buffer + 12, size);
		//}
		m_LePacks--;										//剩余包--
		if (!m_LePacks)									//检查是否接受完毕,接收完毕就关闭文件流
		{
			delete [] m_pIsFileRecv;
			m_pIsFileRecv = nullptr;
			m_File.close();
		}
		else                                                        //如果没有接收完毕，检查刚才接受的是不是最后一个包
		{
			m_pIsFileRecv[pack] = true;	 //标记已经收到该包
			m_LastPackSub = pack;
			if (pack == m_AllPacks)
			{
				//UNDO:当接受到最后一个包发现前面还有没接收的包
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
