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

	//ö�٣��ļ�����
	enum FileType
	{
		File,
		Folder
	};
	//�ļ���Ϣ
	struct FileMsg
	{
		//����
		std::string name;
		//����
		FileType type;
		//��С
		UINT64 size;

		FileMsg() {}
		//����size�Զ�����type
		FileMsg(std::string & tname, UINT64 tsize) :name(tname), size(tsize) { type = tsize ? FileType::File : FileType::Folder; }
		//����size�Զ�����type
		FileMsg(std::string && tname, UINT64 tsize) :name(tname), size(tsize) { type = tsize ? FileType::File : FileType::Folder; }
		//�ֶ�����type
		FileMsg(std::string & tname, FileType ttype, UINT64 tsize) :name(tname), type(ttype), size(tsize) {}
		//�ֶ�����type
		FileMsg(std::string && tname, FileType ttype, UINT64 tsize) :name(tname), type(ttype), size(tsize) {}
	};
	//ö�٣�����ģʽ
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
	//ö�٣���¼���
	enum class LoginType
	{
		Success,
		WrongPassword,
		NoneUser
	};
	//ö�٣�ע����
	enum class RegisterType
	{
		Success,
		InvCodeUsed,
		UserExisted
	};
	//ö�٣��ļ���������
	enum class FileAskType
	{
		None,						//�ļ�������
		Different,				//�ļ����޸�
		OK							//��������
	};

	//��ȡ�ļ����µ������ļ���
	void GetAllFiles(_In std::string path, _Out std::vector <FileMsg> & names);

	//����������ͻ��˵�ϵͳ
	class Server_ForeClient
	{
	private:
		//���߳�������־
		bool m_IsWorking;
		//����Ŀ¼
		std::string m_Path;
		//��ɵ�¼����
		int m_IsLoginSuccess;
		//����ͻ��˵�socket
		SOCKET m_Socket;
		//����ͻ��˵�ip��Ϣ
		sockaddr m_Saddr;
		//recv�߳�
		std::thread m_RecvThread;
		//������
		char m_Buffer[1024];
		//�����Ƿ�ʹ�õı�־
		bool * m_pIsUse;
		//�Ƿ��ڵȴ���������Ϣ
		bool m_IsWaiting;
		//���յ���������
		int m_RecvN;
		//�����ļ���
		bool * m_IsFileUsing;
		//�ϴ��յ������ݰ�����
		RecvType m_LastPackType;

	public:
		Server_ForeClient(SOCKET & csocket, sockaddr & csaddr, bool * isuse, bool * isFileUsing);
		~Server_ForeClient();

	private:
		//�߼�˳��
		void MainDeal();
		//�����̺߳���
		void Recevid();
		//�Զ���ȴ�
		bool WaitTime(DWORD time);
		//����������
		int BufferProcess();
		//���߼�⺯��
		void IsOnlineCheck();

		//��ϸ�Ĵ�����
		//��¼
		LoginType Login(_In std::string name, _In std::string password);
		//ע��
		RegisterType Register(_In std::string name, _In std::string password, _In std::string invitationCode);
		//��ȡ��ǰ�ļ����ļ�
		void FileList();
		//�ͻ������������ļ�
		FileAskType FileBegin(_Out std::string & filePath);
		//ѭ�������ļ�����
		bool SendFile(_In std::string & filePath);
	};

	struct Server
	{
		Server_ForeClient * pData;
		bool isUse;
		Server() :isUse(true) {}
		Server(Server_ForeClient * pD) :pData(pD), isUse(true) {}
	};

	//�����շ�ϵͳ
	class Server_Terminal
	{
	private:
		//ÿ���ͻ�������
		std::vector <Server>m_Clients;
		//����sock
		SOCKET m_ListenSock;
		//������
		char m_Buffer[1024];
		//�ļ���
		bool m_FileUsing[2];
	
	public:
		Server_Terminal();
		~Server_Terminal();
		//����������
		void Work();
		//
	};
}