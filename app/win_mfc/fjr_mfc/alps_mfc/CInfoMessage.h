#pragma once


// CInfoMessage ダイアログ

class CInfoMessage : public CDialogEx
{
	DECLARE_DYNAMIC(CInfoMessage)

public:
	CInfoMessage(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CInfoMessage();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INFOMSG };
#endif
	void SetTitleMessage(const CString& str) { m_title = str; }
private:
	CString m_title;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
