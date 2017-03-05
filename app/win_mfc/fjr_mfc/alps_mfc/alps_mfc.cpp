
// alps_mfc.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "alps_mfc.h"
#include "alps_mfcDlg.h"

/*!	@file alps_mfc.cpp application main.
 *	Copyright(c) sutezo9@me.com 2012.
 */

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern "C" int rd(unsigned char* buffer, unsigned enclen);
static bool res2tmpfile(LPCTSTR destFile, LPBYTE p_src, DWORD dwSize);

// Calps_mfcApp

BEGIN_MESSAGE_MAP(Calps_mfcApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


int g_tax;	// global

// Calps_mfcApp コンストラクション

Calps_mfcApp::Calps_mfcApp()
{
	// 再起動マネージャーをサポートします
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の Calps_mfcApp オブジェクトです。

Calps_mfcApp theApp;


// Calps_mfcApp 初期化

BOOL Calps_mfcApp::InitInstance()
{
	_tsetlocale(LC_ALL, _T(""));	// tstring

	// アプリケーション マニフェストが visual スタイルを有効にするために、
	// ComCtl32.dll Version 6 以降の使用を指定する場合は、
	// Windows XP に InitCommonControlsEx() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// アプリケーションで使用するすべてのコモン コントロール クラスを含めるには、
	// これを設定します。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	int dbid = IDR_DB2017;
	LPTSTR opt = _tcsstr(m_lpCmdLine, _T("-tax"));
	if (NULL != opt) {
		g_tax = _ttoi(opt + 4);
		if (g_tax <= 0) {
			g_tax = DEFAULT_TAX;
		} else if (g_tax == 5) {
			dbid = IDR_DB2014;
		}
	} else {
		g_tax = DEFAULT_TAX;	/* default */
	}

	opt = _tcsstr(m_lpCmdLine, _T("2014"));
	if (NULL != opt) {
		dbid = IDR_DB2014;
	} else {
		opt = _tcsstr(m_lpCmdLine, _T("tax5"));
		if (NULL != opt) {
			dbid = IDR_DB2014;
		} else {
			opt = _tcsstr(m_lpCmdLine, _T("2015"));
			if (NULL != opt) {
				dbid = IDR_DB2015;
			}
		}
	}

	AfxEnableControlContainer();

	// ダイアログにシェル ツリー ビューまたはシェル リスト ビュー コントロールが
	// 含まれている場合にシェル マネージャーを作成します。
	//CShellManager *pShellManager = new CShellManager;

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。
	// SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));

	setlocale(LC_ALL, "");
	
	do {
		HRSRC hRc = FindResource(m_hInstance, MAKEINTRESOURCE(dbid), _T("DB"));
		if (hRc) {
			HGLOBAL hGlbl = LoadResource(m_hInstance, hRc);
			if (hGlbl) {
				DWORD dwDbEncSize = SizeofResource(m_hInstance, hRc);
				if (dwDbEncSize) {
					LPVOID p = LockResource(hGlbl);
					if (p) {
						TCHAR impdbfn[MAX_PATH];
						unsigned char* np = new unsigned char[(dwDbEncSize + 15) & ~15];
						memcpy(np, p, dwDbEncSize);
						UnlockResource(hGlbl);

						// restore(decrypt)
						dwDbEncSize = rd((unsigned char*)np, dwDbEncSize);
						
						GetTempPath(MAX_PATH, impdbfn);
						GetTempFileName(impdbfn, _T("@~~"), 0, impdbfn);
						res2tmpfile(impdbfn, (LPBYTE)np, dwDbEncSize);
						delete[] np;
						bool b = DBS::getInstance()->open(impdbfn);
						DeleteFile(impdbfn);
						if (b) {
							break;	/* success */
						}
					}
				}
			}
		}

		AfxMessageBox(_T("データが開けません."));
		return FALSE;

	} while (0);
	

	Calps_mfcDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//  記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <キャンセル> で消された時のコードを
		//  記述してください。
	}

	// 上で作成されたシェル マネージャーを削除します。
	//if (pShellManager != NULL)
	//{
	//	delete pShellManager;
	//}

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	//  アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}

// create tempolary file for sqlite3 database engine.
//
static bool res2tmpfile(LPCTSTR destFile, LPBYTE p_src, DWORD dwSize)
{
	HANDLE hFile;
	DWORD dwWritten;

	hFile = CreateFile(destFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		TRACE("Can't create file: %d\n", GetLastError());
		return false;
	}
	dwWritten = 0;
	if (!WriteFile(hFile, p_src, dwSize, &dwWritten, NULL) || dwSize != dwWritten) {
		TRACE("Can't write file: %d(write size=%u)\n", GetLastError(), dwWritten);
		return false;
	}
	CloseHandle(hFile);
	return true;
}


