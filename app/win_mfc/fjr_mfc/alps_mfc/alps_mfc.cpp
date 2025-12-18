
// alps_mfc.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//
//  This file is Shift_JIS(Japanese) encoded.

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

// Calps_mfcApp �R���X�g���N�V����

Calps_mfcApp::Calps_mfcApp()
{
	// �ċN���}�l�[�W���[���T�|�[�g���܂�
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}


// �B��� Calps_mfcApp �I�u�W�F�N�g�ł��B

Calps_mfcApp theApp;


// Calps_mfcApp ������

BOOL Calps_mfcApp::InitInstance()
{
	_tsetlocale(LC_ALL, _T(""));	// tstring

	// �A�v���P�[�V���� �}�j�t�F�X�g�� visual �X�^�C����L���ɂ��邽�߂ɁA
	// ComCtl32.dll Version 6 �ȍ~�̎g�p���w�肷��ꍇ�́A
	// Windows XP �� InitCommonControlsEx() ���K�v�ł��B�����Ȃ���΁A�E�B���h�E�쐬�͂��ׂĎ��s���܂��B
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �A�v���P�[�V�����Ŏg�p���邷�ׂẴR���� �R���g���[�� �N���X���܂߂�ɂ́A
	// �����ݒ肵�܂��B
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	int dbid;
	LPTSTR opt;
	
	g_tax = DEFAULT_TAX;

	opt = _tcsstr(m_lpCmdLine, _T("tax5"));
	if (NULL != opt) {
		g_tax = 5;
	} else {
		opt = _tcsstr(m_lpCmdLine, _T("tax8"));
		if (NULL != opt) {
			g_tax = 8;
		} else {
			opt = _tcsstr(m_lpCmdLine, _T("tax10"));
			if (NULL != opt) {
				g_tax = 10;
			}
		}
	}
	
	opt = _tcsstr(m_lpCmdLine, _T("2014"));
	if (NULL != opt) {
		dbid = IDR_DB2014;
		if ((g_tax == DEFAULT_TAX) || (g_tax != 5)) {
			g_tax = 8;
		}
	} else {
		opt = _tcsstr(m_lpCmdLine, _T("2015"));
		if (NULL != opt) {
			dbid = IDR_DB2015;
			g_tax = 8;
		} else {
			opt = _tcsstr(m_lpCmdLine, _T("2017"));
			if (NULL != opt) {
				dbid = IDR_DB2017;
				g_tax = 8;
			} else {
				// default
				dbid = IDR_DBNEWEST;
				if (g_tax != 5) {
					g_tax = DEFAULT_TAX;
				}
			}
		}
	}

	AfxEnableControlContainer();

	// MFC �R���g���[���Ńe�[�}��L���ɂ��邽�߂ɁA"Windows �l�C�e�B�u" �̃r�W���A�� �}�l�[�W���[���A�N�e�B�u��
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// �_�C�A���O�ɃV�F�� �c���[ �r���[�܂��̓V�F�� ���X�g �r���[ �R���g���[����
	// �܂܂�Ă���ꍇ�ɃV�F�� �}�l�[�W���[���쐬���܂��B
	//CShellManager *pShellManager = new CShellManager;

	// �W��������
	// �����̋@�\���g�킸�ɍŏI�I�Ȏ��s�\�t�@�C����
	// �T�C�Y���k���������ꍇ�́A�ȉ�����s�v�ȏ�����
	// ���[�`�����폜���Ă��������B
	// �ݒ肪�i�[����Ă��郌�W�X�g�� �L�[��ύX���܂��B
	// TODO: ��Ж��܂��͑g�D���Ȃǂ̓K�؂ȕ������
	// ���̕������ύX���Ă��������B
	// SetRegistryKey(_T("�A�v���P�[�V���� �E�B�U�[�h�Ő������ꂽ���[�J�� �A�v���P�[�V����"));

	//setlocale(LC_ALL, "");
	
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
						// �Í�����߂� Excel��߂�   
						//dwDbEncSize = rd((unsigned char*)np, dwDbEncSize);
						
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

		AfxMessageBox(_T("�f�[�^���J���܂���."));
		return FALSE;

	} while (0);
	

	Calps_mfcDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �_�C�A���O�� <OK> �ŏ����ꂽ���̃R�[�h��
		//  �L�q���Ă��������B
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �_�C�A���O�� <�L�����Z��> �ŏ����ꂽ���̃R�[�h��
		//  �L�q���Ă��������B
	}

	// ��ō쐬���ꂽ�V�F�� �}�l�[�W���[���폜���܂��B
	//if (pShellManager != NULL)
	//{
	//	delete pShellManager;
	//}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif
	// �_�C�A���O�͕����܂����B�A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n���Ȃ���
	//  �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
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


