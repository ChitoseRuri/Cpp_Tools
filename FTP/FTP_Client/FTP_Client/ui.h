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

		//确定服务器地址，成功了就打开Main（）
		ControlStatus ConnectMenu(_Out std::string & serverIP);
		//主界面
		MenuList Main();
		//登录界面
		ControlStatus Login(_Out std::string & name, _Out std::string & password);
		//注册界面
		ControlStatus Register(_Out std::string & name, _Out std::string & password, _Out string & invitationCode);
		//显示文件仓库
		UINT32 ShowFolder(_In std::vector<FileMsg> & fMsg, _In const std::string & rootString);
		//确认下载文件
		bool ConfirmDownLoad(_In FileMsg & fmsg);
		//下载文件界面
		void DownLoading_1(_In FileMsg & fmsg);
		//下载进度显示
		bool DownLoading_2(_In FileMsg & fmsg, _In unsigned addSize);

	private:
		//输入字的颜色，布局颜色，输入长度，输出字符串，按键显示替代符号（0表示不使用）
		ControlStatus Input(_In int wordColor, _In int backColor, _In int maxLength, _Out std::string & str, _In char out = 0);
		//画文件列表
		inline void DrawFolder(_In std::vector<FileMsg> & fMsg, _In unsigned page, _In const std::string & rootString);
		//设置光标是否显示
		void SetCursorVisible(bool isVisible);
		/*
		设置窗体的大小
		x 是一行显示的char的个数
		y 是行的个数
		*/
		void SetFormSize(_In unsigned x, _In unsigned y);
	};
}