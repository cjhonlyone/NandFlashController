
// USBHOSTAPP.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUSBHOSTAPPApp: 
// �йش����ʵ�֣������ USBHOSTAPP.cpp
//

class CUSBHOSTAPPApp : public CWinApp
{
public:
	CUSBHOSTAPPApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CUSBHOSTAPPApp theApp;