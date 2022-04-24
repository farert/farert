// CQueryNeerest.cpp : 実装ファイル
//

#include "stdafx.h"
#include "alps_mfc.h"
#include "CQueryNeerest.h"
#include "afxdialogex.h"


// CQueryNeerest ダイアログ

IMPLEMENT_DYNAMIC(CQueryNeerest, CDialogEx)

CQueryNeerest::CQueryNeerest(int pass_route, int initialSel, CWnd* pParent /*=nullptr*/)
	: CDialogEx(CQueryNeerest::IDD, pParent)
{
	m_choice = initialSel;
	m_pass_route = pass_route;
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

	// 0 alloff(在来線のみなので選択肢なし)(ここへは来ない)
	// 1 会社線OFF(新幹線はあるが会社線はない)
	// 2 新幹線OFF(会社線はあるが新幹線はない)
	// 3 そのまま(新幹線も会社線もある)(OFFにしない)
	if (m_pass_route == 2) {
		GetDlgItem(IDC_CHK_SHINKANSEN)->EnableWindow(FALSE);
	} else if (m_pass_route == 1) {
		GetDlgItem(IDC_CHK_COMPANY)->EnableWindow(FALSE);
	} else {
		// 3
	}
	if ((m_pass_route != 2) && (m_choice == 1 || m_choice == 3)) {
		((CButton*)GetDlgItem(IDC_CHK_SHINKANSEN))->SetCheck(BST_CHECKED);
	}
	if ((m_pass_route != 1) && (m_choice == 2 || m_choice == 3)) {
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
			m_choice = 3;	// 新幹線も在来線も使う
		}
		else {
			m_choice = 1;	// 新幹線をつかう
		}
	}
	else if (((CButton*)GetDlgItem(IDC_CHK_COMPANY))->GetCheck()
		== BST_CHECKED) {
		m_choice = 2;	// 会社線をつかう
	}
	else {
		m_choice = 0;	// 在来線のみ
	}
	CDialogEx::OnOK();
}
