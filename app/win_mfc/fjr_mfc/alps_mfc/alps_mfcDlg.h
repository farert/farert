
// alps_mfcDlg.h : ヘッダー ファイル
//

/*!	@file alps_mfc.cpp application main.
 *	Copyright(c) sutezo9@me.com 2012.
 */

#pragma once

#include "../../../alps/alpdb.h"
#include "afxmenubutton.h"



// Calps_mfcDlg ダイアログ
class Calps_mfcDlg : public CDialogEx
{
// コンストラクション
public:
	Calps_mfcDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
	enum { IDD = IDD_ALPS_MFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	void routeEnd();

protected:
	enum SELMODE { SEL_LINE, SEL_JUNCTION, SEL_TERMINATE };
	void setupForLinelistByStation(int stationId, int curLineId = 0);
	void setupForStationlistByLine(int lineId, int curStationId, int startStationId);
	int UpdateRouteList();
	int ModifyRouteList();
	UINT CancelRouteQuery();
	void CantSameStartAndEnd();
	void ResetContent();
	void showFare(bool bResetOption = true);
	int parseAndSetupRoute(LPCTSTR route_str);
	void resetMenu();

private:
	Route	m_route;
	Calps_mfcDlg::SELMODE m_selMode;
	int		m_curStationId;
	tstring m_lastRouteString;
	tstring m_lastResultString;

	CMFCMenuButton m_fareOptionMenuButton;
	CMenu   m_menu;

protected:
	afx_msg void OnBnClickedButtonStartsel();
	afx_msg void OnBnClickedButtonEndsel();
	afx_msg void OnBnClickedButtonSel();
	afx_msg void OnBnClickedButtonBs();
	afx_msg void OnNMDblclkListStations(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioBranchSel();
	afx_msg void OnBnClickedRadioTerminalSel();
	afx_msg void OnBnClickedButtonAllClear();
	afx_msg void OnBnClickedButtonTermClear();
	afx_msg void OnBnClickedButtonStart8x();
	afx_msg void OnBnClickedButtonTerminal8x();
	afx_msg void OnBnClickedButtonAutoroute();
public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButtonReverse();
	afx_msg void OnBnClickedButtonRoutecopy();
	afx_msg void OnBnClickedButtonRoutein();
	afx_msg void OnBnClickedButtonResultcopy();
	afx_msg void OnBnClickedButtonRsltopen();
	afx_msg void OnBnClickedButtonRouteOpen();
	afx_msg void OnBnClickedButtonNeerest();
	afx_msg void OnBnClickedMfcmenubuttonFareopt();
	afx_msg void OnStnClickedStaticBar();
 };

