#pragma once

#include <Winsock2.h>
#include <sys/types.h>       
#include <WS2tcpip.h>
#include <Windows.h>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <io.h>
#include <cstdio>
#include <direct.h>
#if defined(DEBUG)||defined(_DEBUG)
#else
#define NDEBUG
#endif
#include <assert.h>

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"ws2_32.lib")

#define _In
#define _Out
#define _RootFolder "Data\\"

namespace CR
{
	using std::string;

	//枚举：文件类型
	enum FileType
	{
		File,
		Folder
	};
	//文件信息
	struct FileMsg
	{
		//名称
		std::string name;
		//类型
		FileType type;
		//大小
		UINT64 size;

		FileMsg() {}
		//根据size自动生成type
		FileMsg(std::string & tname, UINT64 tsize) :name(tname), size(tsize) { type = tsize ? FileType::File : FileType::Folder; }
		//根据size自动生成type
		FileMsg(std::string && tname, UINT64 tsize) :name(tname), size(tsize) { type = tsize ? FileType::File : FileType::Folder; }
		//手动输入type
		FileMsg(std::string & tname, FileType ttype, UINT64 tsize) :name(tname), type(ttype), size(tsize) {}
		//手动输入type
		FileMsg(std::string && tname, FileType ttype, UINT64 tsize) :name(tname), type(ttype), size(tsize) {}
	};
	//枚举：接收模式
	enum class RecvType
	{
		Close,
		Login,
		Register,
		Keep,
		List,
		File_Begin,
		File_Keep,
		File_Last
	};
	//枚举：登录结果
	enum class LoginType
	{
		Success,
		WrongPassword,
		NoneUser
	};
	//枚举：注册结果
	enum class RegisterType
	{
		Success,
		InvCodeUsed,
		UserExisted
	};
	//枚举：文件下载请求
	enum class FileAskType
	{
		None,						//文件不存在
		Different,				//文件已修改
		OK							//允许下载
	};

	//获取文件夹下的所有文件名
	void GetAllFiles(_In std::string path, _Out std::vector <FileMsg> & names);

	//分配给各个客户端的系统
	class Server_ForeClient
	{
	private:
		//多线程启动标志
		bool m_IsWorking;
		//运行目录
		std::string m_Path;
		//完成登录操作
		int m_IsLoginSuccess;
		//这个客户端的socket
		SOCKET m_Socket;
		//这个客户端的ip信息
		sockaddr m_Saddr;
		//recv线程
		std::thread m_RecvThread;
		//缓冲区
		char m_Buffer[1024];
		//该类是否被使用的标志
		bool * m_pIsUse;
		//是否在等待接收新信息
		bool m_IsWaiting;
		//接收到的数据量
		int m_RecvN;
		//共享文件锁
		bool * m_IsFileUsing;
		//上次收到的数据包类型
		RecvType m_LastPackType;

	public:
		Server_ForeClient(SOCKET & csocket, sockaddr & csaddr, bool * isuse, bool * isFileUsing);
		~Server_ForeClient();

	private:
		//逻辑顺序
		void MainDeal();
		//接收线程函数
		void Recevid();
		//自定义等待
		bool WaitTime(DWORD time);
		//缓冲区处理
		int BufferProcess();
		//在线检测函数
		void IsOnlineCheck();

		//详细的处理函数
		//登录
		LoginType Login(_In std::string name, _In std::string password);
		//注册
		RegisterType Register(_In std::string name, _In std::string password, _In std::string invitationCode);
		//获取当前文件夹文件
		void FileList();
		//客户端请求下载文件
		FileAskType FileBegin(_Out std::string & filePath);
		//循环发送文件函数
		bool SendFile(_In std::string & filePath);
	};

	struct Server
	{
		Server_ForeClient * pData;
		bool isUse;
		Server() :isUse(true) {}
		Server(Server_ForeClient * pD) :pData(pD), isUse(true) {}
	};

	//中心收发系统
	class Server_Terminal
	{
	private:
		//每个客户端连接
		std::vector <Server>m_Clients;
		//监听sock
		SOCKET m_ListenSock;
		//缓冲区
		char m_Buffer[1024];
		//文件锁
		bool m_FileUsing[2];
	
	public:
		Server_Terminal();
		~Server_Terminal();
		//服务器启动
		void Work();
		//
	};
}