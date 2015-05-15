#if 0
'Farert'
Copyright (C) 2014 Sutezo (sutezo666@gmail.com)

   'Farert' is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    'Farert' is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with 'Farert'.  If not, see <http://www.gnu.org/licenses/>.

���̃v���O�����̓t���[�\�t�g�E�F�A�ł��B���Ȃ��͂�����A�t���[�\�t�g�E�F
�A���c�ɂ���Ĕ��s���ꂽ GNU ��ʌ��O���p�����_��(�o�[�W����3���A��
�]�ɂ���Ă͂���ȍ~�̃o�[�W�����̂����ǂꂩ)�̒�߂�����̉��ōĔЕz
�܂��͉��ς��邱�Ƃ��ł��܂��B

���̃v���O�����͗L�p�ł��邱�Ƃ�����ĔЕz����܂����A*�S���̖��ۏ�* 
�ł��B���Ɖ\���̕ۏ؂����̖ړI�ւ̓K�����́A���O�Ɏ����ꂽ���̂���
�ߑS�����݂��܂���B�ڂ�����GNU ��ʌ��O���p�����_�񏑂��������������B
 
���Ȃ��͂��̃v���O�����Ƌ��ɁAGNU ��ʌ��O���p�����_�񏑂̕��������ꕔ
�󂯎�����͂��ł��B�����󂯎���Ă��Ȃ���΁A�t���[�\�t�g�E�F�A���c��
�Ő������Ă�������

#endif

// alps_mfc.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
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


	LPTSTR opt = _tcsstr(m_lpCmdLine, _T("-tax"));
	if (NULL != opt) {
		g_tax = _ttoi(opt + 4);
		if (g_tax <= 0) {
			g_tax = DEFAULT_TAX;
		}
	} else {
		g_tax = DEFAULT_TAX;	/* default */
	}

	AfxEnableControlContainer();

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

	setlocale(LC_ALL, "");
	
	do {
		HRSRC hRc = FindResource(m_hInstance, MAKEINTRESOURCE(IDR_DB), _T("DB"));
		if (hRc) {
			HGLOBAL hGlbl = LoadResource(m_hInstance, hRc);
			if (hGlbl) {
				DWORD dwDbEncSize = SizeofResource(m_hInstance, hRc);
				if (dwDbEncSize) {
					LPVOID p = LockResource(hGlbl);
					if (p) {
						TCHAR impdbfn[MAX_PATH];
						
							// restore(decrypt)
						dwDbEncSize = rd((unsigned char*)p, dwDbEncSize);
						
						GetTempPath(MAX_PATH, impdbfn);
						GetTempFileName(impdbfn, _T("@~~"), 0, impdbfn);
						res2tmpfile(impdbfn, (LPBYTE)p, dwDbEncSize);
						
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

