// CInfoMessage.cpp : 実装ファイル
//

#include "stdafx.h"
#include "alps_mfc.h"
#include "CInfoMessage.h"
#include "afxdialogex.h"


// CInfoMessage ダイアログ

IMPLEMENT_DYNAMIC(CInfoMessage, CDialogEx)

CInfoMessage::CInfoMessage(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INFOMSG, pParent)
{

}

CInfoMessage::~CInfoMessage()
{
}

void CInfoMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInfoMessage, CDialogEx)
END_MESSAGE_MAP()


// CInfoMessage メッセージ ハンドラー


BOOL CInfoMessage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_STATIC_TITLE)->SetWindowText(m_title);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void CInfoMessage::OnOK()
{
	if (BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHKNOMSG))->GetCheck()) {
		CDialogEx::OnOK();
	}
	else {
		CDialogEx::OnCancel();
	}
}
