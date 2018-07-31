#pragma once

#include <Winsock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <io.h>
#include <direct.h>

#if defined(DEBUG)||defined (_DEBUG)
#else 
#define NDEBUG
#endif
#include <assert.h>

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"ws2_32.lib")

#define _In
#define _Out
#define _Login_Length 68
#define _Register_Length 84

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
	//枚举：文件下载请求结果
	enum class FileAskType
	{
		Different = -1,				//文件已修改
		None = 0,						//文件不存在
		OK							//允许下载
	};

	class Client_Transport
	{
	private:
		//连接socket
		SOCKET m_Socket;
		//服务器地址信息
		sockaddr_in m_ServAddr;
		//服务器ip信息
		std::string m_ServIpv4;
		//缓冲区
		char m_Buffer[2048];
		//接收线程
		std::thread m_RecvTh;
		//接收到的数据量
		int m_RecvN;
		//是否正在接收服务器信息的标志
		bool m_IsWaiting;
		//文件信息记录
		FileMsg m_FileMsg;
		//文件流
		std::fstream m_File;
		//接受包确认
		bool * m_pIsFileRecv;
		//上一个包
		INT32 m_LastPackSub;
		//剩余包
		INT32 m_LePacks;
		//包总数
		INT32 m_AllPacks;
		//根目录文件夹延续
		std::string m_RootM;
		//当前文件夹文件列表
		std::vector<FileMsg> m_FileList;
		//上次收到的数据包类型
		RecvType m_LastPackType;
		//工作判断符
		bool m_IsWorking;
		//运行目录
		std::string m_Path;

	public:
		Client_Transport();
		~Client_Transport();
		//连接服务器
		bool Connect(_In std::string & ip);
		//登录界面
		bool Login(_In std::string & name, _In std::string & password);
		//注册界面
		void Register(_In std::string & name, _In std::string & password, _In std::string invitationCode);
		//获取仓库信息
		const std::vector<CR::FileMsg> & GetDepotMessage();
		//更新仓库信息
		void  UpdateDepot(_In INT32 sub);
		//请求下载某个文件
		bool DownLoadStart(_In INT32 sub);
		//下载的过程
		INT32 DownLoading();
		//获得根目录地址
		const std::string & GetRootMString();
		
	private:
		//接收线程函数
		void Recevid();
		//自定义等待
		bool WaitTime(DWORD time);
		//缓冲区处理
		INT32 BufferProcess();
		//
		void SetIsWaiting(bool is);
	};
}