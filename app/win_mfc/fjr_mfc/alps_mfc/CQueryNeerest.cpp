// CQueryNeerest.cpp : 実装ファイル
//

#include "stdafx.h"
#include "alps_mfc.h"
#include "CQueryNeerest.h"
#include "afxdialogex.h"


// CQueryNeerest ダイアログ

IMPLEMENT_DYNAMIC(CQueryNeerest, CDialogEx)

CQueryNeerest::CQueryNeerest(int initialSel, CWnd* pParent /*=nullptr*/)
	: CDialogEx(CQueryNeerest::IDD, pParent)
{
	choice = initialSel;
}

CQueryNeerest::~CQueryNeerest()
{
}

void CQueryNeerest::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CQueryNeerest, CDialogEx)
	ON_BN_CLICKED(IDOK, &CQueryNeerest::OnBnClickedOk)
END_MESSAGE_MAP()


// CQueryNeerest メッセージ ハンドラー

BOOL CQueryNeerest::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_STATIC_TITLE)->SetWindowTextW(target);

	((CButton*)GetDlgItem(IDC_CHK_SHINKANSEN))->SetCheck(BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_CHK_COMPANY))->SetCheck(BST_UNCHECKED);

	if (choice == 1 || choice == 3) {
		((CButton*)GetDlgItem(IDC_CHK_SHINKANSEN))->SetCheck(BST_CHECKED);
	}
	if (choice == 2 || choice == 3) {
		((CButton*)GetDlgItem(IDC_CHK_COMPANY))->SetCheck(BST_CHECKED);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CQueryNeerest::OnBnClickedOk()
{
	// 新幹線を使う
	if (((CButton*)GetDlgItem(IDC_CHK_SHINKANSEN))->GetCheck()
		== BST_CHECKED) {
		// 会社線
		if (((CButton*)GetDlgItem(IDC_CHK_COMPANY))->GetCheck()
			== BST_CHECKED) {
			choice = 3;	// 新幹線も在来線も使う
		}
		else {
			choice = 1;	// 新幹線をつかう
		}
	}
	else if (((CButton*)GetDlgItem(IDC_CHK_COMPANY))->GetCheck()
		== BST_CHECKED) {
		choice = 2;	// 会社線をつかう
	}
	else {
		choice = 0;	// 在来線のみ
	}
	CDialogEx::OnOK();
}
