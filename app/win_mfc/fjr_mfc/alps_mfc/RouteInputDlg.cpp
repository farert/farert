// RouteInputDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "alps_mfc.h"
#include "RouteInputDlg.h"
#include "afxdialogex.h"


// CRouteInputDlg ダイアログ

IMPLEMENT_DYNAMIC(CRouteInputDlg, CDialogEx)

CRouteInputDlg::CRouteInputDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRouteInputDlg::IDD, pParent)
	, routeString(_T(""))
{

	routeString = _T("");
}

CRouteInputDlg::~CRouteInputDlg()
{
}

void CRouteInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ROUTE, routeString);
}


BEGIN_MESSAGE_MAP(CRouteInputDlg, CDialogEx)
END_MESSAGE_MAP()


// CRouteInputDlg メッセージ ハンドラー


BOOL CRouteInputDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_EDIT_ROUTE)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
