
// alps_mfc.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//
/*!	@file alps_mfc.h application main.
 *	Copyright(c) sutezo9@me.com 2012.
 */

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル

/*   DB define
 *	../../jr_db_reg.py
 */

#define DEFAULT_TAX		10

extern int g_tax;	// global



// Calps_mfcApp:
// このクラスの実装については、alps_mfc.cpp を参照してください。
//

class Calps_mfcApp : public CWinApp
{
public:
	Calps_mfcApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern Calps_mfcApp theApp;
extern int test_exec(void);

