
// alps_mfcDlg.h : �w�b�_�[ �t�@�C��
//

/*!	@file alps_mfc.cpp application main.
 *	Copyright(c) sutezo9@me.com 2012.
 */

#pragma once

#include "../lib/alps/alpdb.h"



// Calps_mfcDlg �_�C�A���O
class Calps_mfcDlg : public CDialogEx
{
// �R���X�g���N�V����
public:
	Calps_mfcDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^�[

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ALPS_MFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g


// ����
protected:
	HICON m_hIcon;

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
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
	void setupForStationlistByLine(int lineId, int curStationId = 0);
	int UpdateRouteList();
	UINT CancelRouteQuery();
	void CantSameStartAndEnd();
	void ResetContent();
	void showFare();
private:
	Route	m_route;
	Calps_mfcDlg::SELMODE m_selMode;
	int		m_curStationId;

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
	afx_msg void OnBnClickedCheckSpecialCity();
	afx_msg void OnBnClickedCheckRuleapply();
};

