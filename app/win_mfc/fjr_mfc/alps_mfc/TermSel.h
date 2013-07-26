#pragma once



// CTermSel ダイアログ

/*!	@file CTermSel diaglog.
 *	Copyright(c) sutezo9@me.com 2012.
 */

class CTermSel : public CDialogEx
{
	DECLARE_DYNAMIC(CTermSel)

public:
	CTermSel(bool bTerm, CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~CTermSel();

// ダイアログ データ
	enum { IDD = IDD_DIALOG_TERM_SEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	bool m_bTerm;

	int	m_stationId;
	CString m_pszStation;

	DECLARE_MESSAGE_MAP()
	
	DWORD	m_dwSystime;
	DWORD	m_lastListChange;

	void	ClearData();
	void	SetTerminalLineLabel(int staionId);
	void 	saveHistory();
	void 	loadHistory();
public:
	int		getStationId() {
		return m_stationId;
	}
	LPCTSTR getStationName() {
		return m_pszStation;
	}

	afx_msg void OnBnClickedButtonCompanySel();
	afx_msg void OnBnClickedButtonLineSel();
	afx_msg void OnLbnSelchangeListCompany();
	afx_msg void OnLbnDblclkListCompany();
	afx_msg void OnLbnDblclkListLines();
	afx_msg void OnLbnDblclkListTerminals();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonStationSel();
	afx_msg void OnLbnSelchangeListLines();
	afx_msg void OnLbnSelchangeListTerminals();
	afx_msg void OnCbnSelchangeComboStation();
	afx_msg void OnCbnEditchangeComboStation();
	afx_msg void OnCbnSetfocusComboStation();
	afx_msg void OnCbnKillfocusComboStation();
	afx_msg void OnCbnEditupdateComboStation();
	afx_msg void OnCbnSelendokComboStation();
//	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
