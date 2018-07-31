#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#include "client.h"

#define _In
#define _Out

namespace CR
{
#define CONSOLE_TITLE "Chitose Ruri's FTP Client"

	using std::string;
	using std::cin;
	using std::cout;

	enum class MenuList
	{
		Main,
		Login,
		Register
	};

	enum class ControlStatus
	{
		Before,
		Next,
		SuccessOut,
		WrongOut,
		JumpOut
	};

	class Client_UI
	{
	private:
		HANDLE m_Handle;
		unsigned m_FormSizeX, m_FormSizeY;

	public:
		Client_UI();
		~Client_UI();

		//ȷ����������ַ���ɹ��˾ʹ�Main����
		ControlStatus ConnectMenu(_Out std::string & serverIP);
		//������
		MenuList Main();
		//��¼����
		ControlStatus Login(_Out std::string & name, _Out std::string & password);
		//ע�����
		ControlStatus Register(_Out std::string & name, _Out std::string & password, _Out string & invitationCode);
		//��ʾ�ļ��ֿ�
		UINT32 ShowFolder(_In std::vector<FileMsg> & fMsg, _In const std::string & rootString);
		//ȷ�������ļ�
		bool ConfirmDownLoad(_In FileMsg & fmsg);
		//�����ļ�����
		void DownLoading_1(_In FileMsg & fmsg);
		//���ؽ�����ʾ
		bool DownLoading_2(_In FileMsg & fmsg, _In unsigned addSize);

	private:
		//�����ֵ���ɫ��������ɫ�����볤�ȣ�����ַ�����������ʾ������ţ�0��ʾ��ʹ�ã�
		ControlStatus Input(_In int wordColor, _In int backColor, _In int maxLength, _Out std::string & str, _In char out = 0);
		//���ļ��б�
		inline void DrawFolder(_In std::vector<FileMsg> & fMsg, _In unsigned page, _In const std::string & rootString);
		//���ù���Ƿ���ʾ
		void SetCursorVisible(bool isVisible);
		/*
		���ô���Ĵ�С
		x ��һ����ʾ��char�ĸ���
		y ���еĸ���
		*/
		void SetFormSize(_In unsigned x, _In unsigned y);
	};
}