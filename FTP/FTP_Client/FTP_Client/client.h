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
	//ö�٣��ļ�����������
	enum class FileAskType
	{
		Different = -1,				//�ļ����޸�
		None = 0,						//�ļ�������
		OK							//��������
	};

	class Client_Transport
	{
	private:
		//����socket
		SOCKET m_Socket;
		//��������ַ��Ϣ
		sockaddr_in m_ServAddr;
		//������ip��Ϣ
		std::string m_ServIpv4;
		//������
		char m_Buffer[2048];
		//�����߳�
		std::thread m_RecvTh;
		//���յ���������
		int m_RecvN;
		//�Ƿ����ڽ��շ�������Ϣ�ı�־
		bool m_IsWaiting;
		//�ļ���Ϣ��¼
		FileMsg m_FileMsg;
		//�ļ���
		std::fstream m_File;
		//���ܰ�ȷ��
		bool * m_pIsFileRecv;
		//��һ����
		INT32 m_LastPackSub;
		//ʣ���
		INT32 m_LePacks;
		//������
		INT32 m_AllPacks;
		//��Ŀ¼�ļ�������
		std::string m_RootM;
		//��ǰ�ļ����ļ��б�
		std::vector<FileMsg> m_FileList;
		//�ϴ��յ������ݰ�����
		RecvType m_LastPackType;
		//�����жϷ�
		bool m_IsWorking;
		//����Ŀ¼
		std::string m_Path;

	public:
		Client_Transport();
		~Client_Transport();
		//���ӷ�����
		bool Connect(_In std::string & ip);
		//��¼����
		bool Login(_In std::string & name, _In std::string & password);
		//ע�����
		void Register(_In std::string & name, _In std::string & password, _In std::string invitationCode);
		//��ȡ�ֿ���Ϣ
		const std::vector<CR::FileMsg> & GetDepotMessage();
		//���²ֿ���Ϣ
		void  UpdateDepot(_In INT32 sub);
		//��������ĳ���ļ�
		bool DownLoadStart(_In INT32 sub);
		//���صĹ���
		INT32 DownLoading();
		//��ø�Ŀ¼��ַ
		const std::string & GetRootMString();
		
	private:
		//�����̺߳���
		void Recevid();
		//�Զ���ȴ�
		bool WaitTime(DWORD time);
		//����������
		INT32 BufferProcess();
		//
		void SetIsWaiting(bool is);
	};
}