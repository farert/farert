
// alps_mfc.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//
/*!	@file alps_mfc.h application main.
 *	Copyright(c) sutezo9@me.com 2012.
 */

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��

/*   DB define
 *	../../jr_db_reg.py
 */

#define DEFAULT_TAX		10

extern int g_tax;	// global



// Calps_mfcApp:
// ���̃N���X�̎����ɂ��ẮAalps_mfc.cpp ���Q�Ƃ��Ă��������B
//

class Calps_mfcApp : public CWinApp
{
public:
	Calps_mfcApp();

// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern Calps_mfcApp theApp;
extern int test_exec(void);

