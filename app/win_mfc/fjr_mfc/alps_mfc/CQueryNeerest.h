#pragma once


// CQueryNeerest ダイアログ

class CQueryNeerest : public CDialogEx
{
	DECLARE_DYNAMIC(CQueryNeerest)

public:
	CQueryNeerest(int initialSel, CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CQueryNeerest();

// ダイアログ データ
	enum { IDD = IDD_QUERYNEEREST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	int choice;
	CString target;
	afx_msg void OnBnClickedMfcbuttonNormal();
	afx_msg void OnBnClickedMfcbuttonBullet();
	afx_msg void OnBnClickedMfcbuttonCompany();
	afx_msg void OnBnClickedMfcbuttonBulletCompanu();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
