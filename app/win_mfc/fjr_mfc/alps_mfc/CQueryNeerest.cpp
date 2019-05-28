// CQueryNeerest.cpp : 実装ファイル
//

#include "stdafx.h"
#include "alps_mfc.h"
#include "CQueryNeerest.h"
#include "afxdialogex.h"


// CQueryNeerest ダイアログ

IMPLEMENT_DYNAMIC(CQueryNeerest, CDialogEx)

CQueryNeerest::CQueryNeerest(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CQueryNeerest::IDD, pParent)
{

}

CQueryNeerest::~CQueryNeerest()
{
}

void CQueryNeerest::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CQueryNeerest, CDialogEx)
	ON_BN_CLICKED(IDC_MFCBUTTON_NORMAL, &CQueryNeerest::OnBnClickedMfcbuttonNormal)
	ON_BN_CLICKED(IDC_MFCBUTTON_BULLET, &CQueryNeerest::OnBnClickedMfcbuttonBullet)
	ON_BN_CLICKED(IDC_MFCBUTTON_COMPANY, &CQueryNeerest::OnBnClickedMfcbuttonCompany)
	ON_BN_CLICKED(IDC_MFCBUTTON_BULLET_COMPANU, &CQueryNeerest::OnBnClickedMfcbuttonBulletCompanu)
END_MESSAGE_MAP()


// CQueryNeerest メッセージ ハンドラー


// 在来線のみ
void CQueryNeerest::OnBnClickedMfcbuttonNormal()
{
	choice = 0;
	CDialogEx::OnOK();
}

// 新幹線を使う
void CQueryNeerest::OnBnClickedMfcbuttonBullet()
{
	choice = 1;
	CDialogEx::OnOK();
}

// 会社線を使う
void CQueryNeerest::OnBnClickedMfcbuttonCompany()
{
	choice = 2;
	CDialogEx::OnOK();
}

// 新幹線と在来線を使う
void CQueryNeerest::OnBnClickedMfcbuttonBulletCompanu()
{
	choice = 3;
	CDialogEx::OnOK();
}


BOOL CQueryNeerest::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_STATIC_TITLE)->SetWindowTextW(target);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 例外 : OCX プロパティ ページは必ず FALSE を返します。
}
