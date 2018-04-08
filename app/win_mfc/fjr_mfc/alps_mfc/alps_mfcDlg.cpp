
// alps_mfcDlg.cpp : �����t�@�C��
//
//	Copyright(c) sutezo9@me.com 2012 allright reserved.
//	This source code is GPL license.

#include "stdafx.h"
#include "alps_mfc.h"
#include "alps_mfcDlg.h"
#include "afxdialogex.h"
#include "TermSel.h"
#include "RouteInputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
#include <vector>

// ���[�g�I��[����]��?
//#define isComplete()	(1 < m_route.routeList().size())



// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CAboutDlg::OnNMClickSyslink1)
END_MESSAGE_MAP()


// Calps_mfcDlg �_�C�A���O




Calps_mfcDlg::Calps_mfcDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Calps_mfcDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Calps_mfcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCMENUBUTTON_FAREOPT, m_fareOptionMenuButton);
}

BEGIN_MESSAGE_MAP(Calps_mfcDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_STARTSEL, &Calps_mfcDlg::OnBnClickedButtonStartsel)
	ON_BN_CLICKED(IDC_BUTTON_SEL, &Calps_mfcDlg::OnBnClickedButtonSel)
	ON_BN_CLICKED(IDC_BUTTON_BS, &Calps_mfcDlg::OnBnClickedButtonBs)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LINESTATIONS, &Calps_mfcDlg::OnNMDblclkListStations)
	ON_BN_CLICKED(IDC_RADIO_BRANCH_SEL, &Calps_mfcDlg::OnBnClickedRadioBranchSel)
	ON_BN_CLICKED(IDC_RADIO_TERMINAL_SEL, &Calps_mfcDlg::OnBnClickedRadioTerminalSel)
	ON_BN_CLICKED(IDC_BUTTON_ALL_CLEAR, &Calps_mfcDlg::OnBnClickedButtonAllClear)
	ON_BN_CLICKED(IDC_BUTTON_AUTOROUTE, &Calps_mfcDlg::OnBnClickedButtonAutoroute)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_REVERSE, &Calps_mfcDlg::OnBnClickedButtonReverse)
	ON_BN_CLICKED(IDC_BUTTON_ROUTECOPY, &Calps_mfcDlg::OnBnClickedButtonRoutecopy)
	ON_BN_CLICKED(IDC_BUTTON_ROUTEIN, &Calps_mfcDlg::OnBnClickedButtonRoutein)
	ON_BN_CLICKED(IDC_BUTTON_RESULTCOPY, &Calps_mfcDlg::OnBnClickedButtonResultcopy)
	ON_BN_CLICKED(IDC_BUTTON_RSLTOPEN, &Calps_mfcDlg::OnBnClickedButtonRsltopen)
	ON_BN_CLICKED(IDC_BUTTON_ROUTE_OPEN, &Calps_mfcDlg::OnBnClickedButtonRouteOpen)
	ON_BN_CLICKED(IDC_BUTTON_NEEREST, &Calps_mfcDlg::OnBnClickedButtonNeerest)
	ON_BN_CLICKED(IDC_MFCMENUBUTTON_FAREOPT, &Calps_mfcDlg::OnBnClickedMfcmenubuttonFareopt)
	ON_STN_CLICKED(IDC_STATIC_BAR, &Calps_mfcDlg::OnStnClickedStaticBar)
END_MESSAGE_MAP()


// Calps_mfcDlg ���b�Z�[�W �n���h���[

BOOL Calps_mfcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	m_menu.LoadMenu(IDR_MENU1);
	m_fareOptionMenuButton.m_hMenu = m_menu.GetSubMenu(0)->GetSafeHmenu();


	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	//--
	DBsys dbsys;
	RouteUtil::DbVer(&dbsys);
	CString s;

	s.Format(_T(" - DB ver[%s(�����%d%%):%sGMT]"), dbsys.name, dbsys.tax, dbsys.createdate);
	CString ss;
	GetWindowText(ss);
	SetWindowText(ss + s);

	CListCtrl* pLSel = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_LINESTATIONS));
	pLSel->SetColumnWidth(pLSel->InsertColumn(0, _T("")), LVSCW_AUTOSIZE);
	pLSel->SetColumnWidth(pLSel->InsertColumn(1, _T("")), LVSCW_AUTOSIZE);

	pLSel = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE));
	CRect rc;
	pLSel->GetWindowRect(rc);
	pLSel->InsertColumn(0, _T("�H��"), LVCFMT_LEFT, rc.Width() / 5  * 3, 0);
	pLSel->InsertColumn(1, _T("�抷�w"), LVCFMT_LEFT, rc.Width() / 5 * 2, 0);

	ResetContent();	// ���w�A���w�A�o�H => �S���� IDC_BUTTON_ALL_CLEAR [X] Button pushed.

//////////////////////////////////////#####################
#ifdef _DEBUG
	test_exec();	// �P�̃e�X�g�����s(���ʂ�test_result.txt)
#endif
//////////////////////////////////////#####################

	DragAcceptFiles();

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

void Calps_mfcDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void Calps_mfcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR Calps_mfcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//	Dialog�Ȃ̂�Enter��ESC�ŃE�B���h�E�����Ȃ��悤�ɂ��邽��.
//
BOOL Calps_mfcDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) {
		case VK_RETURN:
		case VK_ESCAPE:
			return TRUE;
		default:
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


// ���w�w��
//	IDC_BUTTON_STARTSEL [...] button pushed
//
void Calps_mfcDlg::OnBnClickedButtonStartsel()
{
	CTermSel dlg(false, this);
	int startStationId;

	if (IDOK == dlg.DoModal()) {
		startStationId = dlg.getStationId();
		if (startStationId != m_route.startStationId()) {
			// ���I�����[�g�̔j���m�F
			if (0 < reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE))->GetItemCount()) {
				if (IDYES != CancelRouteQuery()) {
					return;
				}
				ResetContent();	// ���w�A���w�A�o�H => �S���� IDC_BUTTON_ALL_CLEAR [X] Button pushed.
			}
			m_curStationId = startStationId;		// ���wId
			m_route.add(startStationId);
			GetDlgItem(IDC_EDIT_START)->SetWindowText(dlg.getStationName());	// ���w�\��
			setupForLinelistByStation(m_curStationId);				// ���w�̏����H���ꗗ�̕\��
			GetDlgItem(IDC_LIST_LINESTATIONS)->EnableWindow(TRUE);	// �w/�H�� �I�����X�g�I����
			GetDlgItem(IDC_BUTTON_SEL)->EnableWindow(TRUE);			// [+] button
			GetDlgItem(IDC_BUTTON_AUTOROUTE)->EnableWindow(TRUE);	/* Enable [AutoRoute]�{�^�� */
		}// �s�ύX�Ȃ�Ȃɂ����Ȃ�
	}
}


// �u�H���v�^�u����w�v�^�u���w�v�̑I��
//	IDC_BUTTON_SEL [+] button pushed.
//
void Calps_mfcDlg::OnBnClickedButtonSel()
{
	int selIdx;
	int selId;
	int nRoute;
	CString selTitle;
	int curLineId;

	int wc;
	int wn;

	CListCtrl* pLRoute = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE));
	CListCtrl* pLSel   = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_LINESTATIONS));

	selIdx   = pLSel->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if ((1 !=  pLSel->GetSelectedCount()) || (-1 == selIdx)) {
		return;		/* Multiselected or non selection. */
	}

	if (m_route.isEnd()) {
		return;		/* already finished */
	}

	if ((pLSel->GetItemText(selIdx, 0))[0] == _T('-')) {
		SetDlgItemText(IDC_EDIT_STAT, _T("���O��ԘH���͎w��ł��܂���.�߂��ď抷�w���đI�����Ă�������."));
		return;
	}

	nRoute	 = pLRoute->GetItemCount();			// �o�H���X�g��
	selTitle = pLSel->GetItemText(selIdx, 0);	// �I�� �w�^�� ��
	selId    = pLSel->GetItemData(selIdx);		//        ��Id

	ASSERT(0 < selId);

	if (m_selMode == SEL_LINE) {
		// add route list �H��Id
		pLRoute->SetItemText(pLRoute->InsertItem(LVIF_TEXT | LVIF_PARAM, nRoute, selTitle, 0, 0, 0, MAKEPAIR(selId, 0)), 1, _T(""));
		wc = pLRoute->GetColumnWidth(0);
		wn = pLRoute->GetStringWidth(selTitle) + 16;
		if (wc < wn) {
			pLRoute->SetColumnWidth(0, wn);
		}
		GetDlgItem(IDC_BUTTON_BS)->EnableWindow(TRUE);		// Enable [-] button

		m_selMode = SEL_JUNCTION;
		setupForStationlistByLine(selId, m_curStationId);
	}
	else {
		// add route list �wId

		m_route.setNotSameKokuraHakataShinZai(
			(MF_CHECKED & m_menu.GetMenuState(IDR_MENU_KOKURA_HAKATA_BULLET_NOTSAME, MF_BYCOMMAND))
				? true : false);

		/* �o�H�ǉ� */// last line
		curLineId = IDENT1(pLRoute->GetItemData(nRoute - 1));	//(stationId is not yet strage in HIWORD)
		TRACE(_T("[route.add(%s-%s)\n"), RouteUtil::LineName(curLineId).c_str(), RouteUtil::StationName(selId).c_str());
		int rslt = m_route.add(curLineId, /*m_curStationId,*/selId);
		if (m_route.isModified()) {
			if (0 < UpdateRouteList()) {		// �o�H�\���X�V
				curLineId = m_route.routeList().back().lineId;
				selId = m_route.routeList().back().stationId;
			}
			else {
				ResetContent();
				return;
			}
		}
		else if ((rslt == 0) || (rslt == 1) || (rslt == 4)) {
			pLRoute->SetItemText(nRoute - 1, 1, selTitle);					// �o�H�F����w�\��
			pLRoute->SetItemData(nRoute - 1, MAKEPAIR(curLineId, selId));	//�o�H�F����wID�ݒ�
																			// �J����������
			wc = pLRoute->GetColumnWidth(1);
			wn = pLRoute->GetStringWidth(selTitle) + 16;
			if (wc < wn) {
				pLRoute->SetColumnWidth(1, wn);
			}
		}
		if (rslt < 0) {
			switch (rslt) {
			case -1:
				SetDlgItemText(IDC_EDIT_STAT, _T("�o�H���d�����Ă��܂�."));
				break;
			case -4:
				SetDlgItemText(IDC_EDIT_STAT, _T("������Ȃ���А��ʉߘA���^�A�ł�."));
				break;
			default:
				ASSERT(FALSE);	/* -1 �ȊO�̓o�O(DB�G���[���A�����s��) */
				AfxMessageBox(_T("Fatal error occured."));
				ResetContent();
				return;
			}
			return;
		}

		if (5 == rslt) {
			SetDlgItemText(IDC_EDIT_STAT, _T("�o�H�͕Г������ɒB���Ă��܂�."));
			return;	/* already finished */
		}
		if ((0 != rslt) && (1 != rslt) && (4 != rslt)) {	/* fin */
			ASSERT(FALSE);
			return;
		}

		/* result == 1 or 0 or 4 */

		setupForLinelistByStation(selId, curLineId);

		/* �^���\�� */
		showFare();
		m_curStationId = selId;
		m_selMode = SEL_LINE;	/* for [-] button */
		ASSERT(m_curStationId == (m_route.routeList().cend() - 1)->stationId);
		GetDlgItem(IDC_EDIT_END)->SetWindowText(RouteUtil::StationName((m_route.routeList().cend() - 1)->stationId).c_str());	// ���w�\��

		if (0 == rslt) {
			SetDlgItemText(IDC_EDIT_STAT, _T("�o�H���Г������ɒB���܂���."));
			MessageBox(_T("�o�H���Г������ɒB���܂���."), _T("�o�H�I�["),
				MB_OK | MB_ICONINFORMATION);
		}
		else if (4 == rslt) {
			SetDlgItemText(IDC_EDIT_STAT, _T("��А����܂ތo�H�͂���ȏ�w��ł��܂���."));
		}
	}
}

//	List double click
//	IDC_LIST_LINESTATIONS [(�H���^����w�^���w)] ListCtrl Double clicked.
//
void Calps_mfcDlg::OnNMDblclkListStations(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedButtonSel();	// �u�H���v/�u����w�v/�u���w�v�̑I��(IDC_BUTTON_SEL[+]button pushed)
}

//	����w�I�� Radio Button
//	IDC_RADIO_BRANCH_SEL [����w] Radio Button pushed.
//
void Calps_mfcDlg::OnBnClickedRadioBranchSel()
{
	int itemcnt;

	if (m_selMode == SEL_LINE) {
		ASSERT(FALSE);
		return;
	}

	if (IDC_RADIO_BRANCH_SEL == GetCheckedRadioButton(IDC_RADIO_BRANCH_SEL, IDC_RADIO_TERMINAL_SEL)) {
		m_selMode = SEL_JUNCTION;
	} else {
		m_selMode = SEL_TERMINATE;
	}

	CListCtrl* pLRoute = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE));
	itemcnt = pLRoute->GetItemCount();
	int curLineId = IDENT1(pLRoute->GetItemData(itemcnt - 1));	// current select line

	ASSERT(((pLRoute->GetItemCount() <= 1) && (m_route.routeList().size() == 0) &&
			(m_curStationId == m_route.startStationId())) ||
			((itemcnt < 2) || (m_curStationId == IDENT2(pLRoute->GetItemData(pLRoute->GetItemCount() - 2)))));	// current select line

	if (0 < curLineId) {
		setupForStationlistByLine(curLineId, m_curStationId);
	} else {
		ASSERT(FALSE);
	}
}

//	�I���w�w�� Radio button
//	IDC_RADIO_STATION_SEL [���w] Radio Button pushed.
//
void Calps_mfcDlg::OnBnClickedRadioTerminalSel()
{
	OnBnClickedRadioBranchSel();	// IDC_RADIO_BRANCH_SEL [����w] Radio Button pushed.
}

// delete button(�o�H�̖������L�����Z��)
//	IDC_BUTTON_BS [-] button pushed.
//
//	enable: IDC_LIST_LINESTATIONS item added for IDC_BUTTON_SEL click or IDC_LIST_LINESTATIONS double click
//	disable: this(self) removed or clear(route_end())
//
void Calps_mfcDlg::OnBnClickedButtonBs()
{
	CListCtrl* pLSel   = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_LINESTATIONS));
	CListCtrl* pLRoute = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE));
	int numList;
	int curLineId;

	numList = pLRoute->GetItemCount();
	if (numList < 1) {
		ASSERT(FALSE);
		return;
	}

	if (m_selMode == SEL_LINE) {
		/* �H�����X�g�\���� */
		ASSERT(2 <= m_route.routeList().size());
		ASSERT(m_curStationId == IDENT2(pLRoute->GetItemData(numList - 1)));

		if (2 <= numList) {			/* Route���X�g�ɕ���w��2�s�ȏ�? */
			m_curStationId = IDENT2(pLRoute->GetItemData(numList - 2)); // �ŏI�s�̕���w
		} else {
			m_curStationId = m_route.startStationId();		// ���w
		}
		m_selMode = SEL_JUNCTION;
		m_route.removeTail();

		curLineId = IDENT1(pLRoute->GetItemData(numList - 1));	// last line
		setupForStationlistByLine(curLineId, m_curStationId);

		pLRoute->SetItemText(numList - 1, 1, _T(""));
		pLRoute->SetItemData(numList - 1, MAKEPAIR(curLineId, 0));

		/* �^���\�� */
		showFare();
		if (m_route.routeList().size() <= 1) {
			GetDlgItem(IDC_EDIT_END)->SetWindowText(_T(""));	// ���w�\��
		}
		else {
			GetDlgItem(IDC_EDIT_END)->SetWindowText(RouteUtil::StationName((m_route.routeList().cend() - 1)->stationId).c_str());	// ���w�\��
		}
	} else {	/* SEL_JUNCTION or SEL_TERMINATE */
		/* ����w�^���w���X�g�\���� */
		m_selMode = SEL_LINE;

		if (2 <= numList) {
			curLineId = IDENT1(pLRoute->GetItemData(numList - 2));	// last line
			ASSERT(0 < curLineId);
		} else {
			curLineId = 0;
			GetDlgItem(IDC_BUTTON_BS)->EnableWindow(FALSE);	// Disable [-] button
		}
		setupForLinelistByStation(m_curStationId, curLineId);

		pLRoute->DeleteItem(numList - 1);				// remvoe tail
	}
}

//	���w�A���w�A�o�H => �S����
//	IDC_BUTTON_ALL_CLEAR [X] Button pushed.
//
void Calps_mfcDlg::OnBnClickedButtonAllClear()
{
	if (0 < reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE))->GetItemCount()) {
		if (IDYES != CancelRouteQuery()) {
			return;
		}
	}
	ResetContent();
}


//	IDC_BUTTON_AUTOROUTE [�ŒZ�o�H] button pushed
//	���w���w�肵�čŒZ�o�H���Z�o
//
void Calps_mfcDlg::OnBnClickedButtonAutoroute()
{
	int rc;
	int endStationId;

	if (m_route.startStationId() <= 0) {
		MessageBox(_T("�J�n�w�A�I���w���w�肵�Ȃ��ƍŒZ�o�H�͎Z�o���܂���."),
										_T("�m�F"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CListCtrl* pLRoute = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE));

	CTermSel dlg(true, this);

	if (IDOK != dlg.DoModal()) {
		return;
	}
	endStationId = dlg.getStationId();

	ASSERT(0 < endStationId);

	// �J�n�w=�I���w�͖Ӓ����łȂ����Ƃ��m�F
	if (endStationId == m_route.startStationId()) {
		if (RouteUtil::NumOfNeerNode(endStationId) <= 1) {
			CantSameStartAndEnd();	// "�w�肵�����w�����w�̌o�H�͎w��s�\�ł�"
			return;
		}
	}

	if (m_route.routeList().back().stationId == endStationId) {
		MessageBox(_T("�J�n�w=�I���w�ł͍ŒZ�o�H�͎Z�o���܂���."),
										_T("�m�F"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	RouteList route(m_route);
	rc = m_route.changeNeerest((IDYES == MessageBox(_T("�V�������܂߂܂���?"),
								_T("�m�F"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2)), endStationId);
	if ((rc == 5) || (rc == 0)) {
		if (m_route.isModified()) {
			UpdateRouteList();
		}
		SetDlgItemText(IDC_EDIT_STAT, _T("�o�H�͕Г������ɒB���Ă��܂�."));
		return;	/* already finished */
	} else if (rc == 4) { /* already routed */
		SetDlgItemText(IDC_EDIT_STAT, _T("�J�n�w�֖߂�ɂ͂��������o�H���w�肵�Ă���ɂ��Ă�������"));
		m_route.assign(route);	/* such as ��X�� �V��v�� -> ��X�� */
		return;
	}
	if (0 < rc) {
		int numList = UpdateRouteList();	/* IDC_LIST_ROUTE update view */
		if (0 < numList) {
			ASSERT(m_curStationId == endStationId);
			return;				/* success */
		} else {
			ASSERT(FALSE);
			m_route.assign(route);
			UpdateRouteList();	/* IDC_LIST_ROUTE update view */
		}
	} else if (-100 < rc) {
		MessageBox(_T("�o�H���d�����Ă��邽�ߎZ�o�ł��܂���ł���."),
				   _T("�������[�g"), MB_OK | MB_ICONEXCLAMATION);
		m_route.assign(route);
		UpdateRouteList();	/* IDC_LIST_ROUTE update view */
	} else { /* < -1000 */
		MessageBox(_T("�Z�o�ł��܂���ł���."),
				   _T("�m�F"), MB_OK | MB_ICONEXCLAMATION);
		m_route.assign(route);
		UpdateRouteList();	/* IDC_LIST_ROUTE update view */
	}
}

//	ListView<IDC_LIST_LINESTATIONS>
//	�w�̏����H���ꗗ��\��
//
//	@param [in]				stationId		�w
//	@param [in][optional]	curLineId	�I��s�H��(���O��ԘH��)
//
void Calps_mfcDlg::setupForLinelistByStation(int stationId, int curLineId /* =0 */)
{
	CListCtrl* pLSel = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_LINESTATIONS));
	LVCOLUMN column;
	int idx;

	pLSel->DeleteAllItems();			// all record clear

	// ����w<->�I���w �I��
	GetDlgItem(IDC_RADIO_BRANCH_SEL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO_TERMINAL_SEL)->ShowWindow(SW_HIDE);

	memset(&column, 0, sizeof(column));

	CRect rc;
	pLSel->GetClientRect(rc);

	column.mask = LVCF_TEXT | LVCF_WIDTH;
	column.pszText = _T("�H��");		// List title
	column.cx = rc.Width();
	pLSel->SetColumn(0, &column);
	column.pszText = _T("");
	column.cx = 10;
	pLSel->SetColumn(1, &column);

	// �w�̏����H�������X�g
	DBO dbo = RouteUtil::Enum_line_of_stationId(stationId);
	if (!dbo.isvalid()) {
		ASSERT(FALSE);
		return;
	}
	for (idx = 0; dbo.moveNext(); idx++) {
		CString lineName;
		int lineId = dbo.getInt(1);

		/* �O��I��H���͑I��s��(�s����"-") */
		lineName.Format(_T("%s%s"), (lineId == curLineId) ? _T("- ") : _T(""), dbo.getText(0).c_str());

		pLSel->SetItemText(
				pLSel->InsertItem(LVIF_TEXT | LVIF_PARAM,
								idx, lineName, 0, 0, 0, lineId),
				1, _T(""));
	}
	ASSERT(1 <= idx);
	SetDlgItemText(IDC_EDIT_STAT, _T("�H����I�����Ă�������."));
	SetDlgItemText(IDC_STATIC_LIST_SEL, _T("�H���I��"));
}

//	ListView<IDC_LIST_LINESTATIONS>
//	�H�����̕���w�^�S�w�ꗗ��\��
//
//	@param [in]				lineId			�H��
//	@param [in][optional]	curStationId	�I��s�w
//
void Calps_mfcDlg::setupForStationlistByLine(int lineId, int curStationId /* =0 */)
{
	CListCtrl* pLSel = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_LINESTATIONS));
	LVCOLUMN column;
	int idx;

	GetDlgItem(IDC_RADIO_BRANCH_SEL)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_RADIO_TERMINAL_SEL)->ShowWindow(SW_SHOW);

	pLSel->DeleteAllItems();			// all record clear

	memset(&column, 0, sizeof(column));

	CRect rc;
	pLSel->GetClientRect(rc);

	column.mask = LVCF_TEXT;

	if (m_selMode == SEL_JUNCTION) {
		CheckRadioButton(IDC_RADIO_BRANCH_SEL, IDC_RADIO_TERMINAL_SEL, IDC_RADIO_BRANCH_SEL);
		column.pszText = _T("����w");
		SetDlgItemText(IDC_EDIT_STAT, _T("�抷�w��I�����Ă�������."));
		SetDlgItemText(IDC_STATIC_LIST_SEL, _T("�抷�w�I��"));
	} else {
		CheckRadioButton(IDC_RADIO_BRANCH_SEL, IDC_RADIO_TERMINAL_SEL, IDC_RADIO_TERMINAL_SEL);
		column.pszText = _T("�w");
		SetDlgItemText(IDC_EDIT_STAT, _T("���w��I�����Ă�������."));
		SetDlgItemText(IDC_STATIC_LIST_SEL, _T("���w�I��"));
	}
	pLSel->SetColumn(0, &column);

	column.pszText = _T("����H��");
	pLSel->SetColumn(1, &column);

	DBO dbo;
	if (m_selMode == SEL_JUNCTION) {
		dbo = RouteUtil::Enum_junction_of_lineId(lineId, -1);
	} else if (m_selMode == SEL_TERMINATE) {
		dbo = RouteUtil::Enum_station_of_lineId(lineId);
	} else {
		ASSERT(FALSE);
		return;
	}
	if (!dbo.isvalid()) {
		ASSERT(FALSE);
		return;
	}

	int width1, width2;		// �J�������v�Z�p�ϐ�
	width1 = width2 = 0;

	for (idx = 0; dbo.moveNext(); idx++) {
		CString stationName;
		int stationId = dbo.getInt(1);

		stationName.Format(_T("%s%s"),
			(stationId == curStationId) ? _T("- ") : _T(""), dbo.getText(0).c_str());

		CString jctLines;
		if (0 != dbo.getInt(2)) { /* ����w */
			// ����w�̏�����H����'/'�ŋ�؂�񋓂�����������쐬
			DBO dbo_lines = RouteUtil::Enum_line_of_stationId(stationId);
			if (!dbo_lines.isvalid()) {
				ASSERT(FALSE);
			} else {
				while (dbo_lines.moveNext()) {
					if (!BIT_CHK(dbo_lines.getInt(2), BSRJCTSP)) {
						if (jctLines != _T("")) {
							jctLines += _T("/");
						}
						jctLines += dbo_lines.getText(0).c_str();
					}
				}
			}
		}
		pLSel->SetItemText(
			pLSel->InsertItem(LVIF_TEXT | LVIF_PARAM,
							idx, stationName, 0, 0, 0, stationId),
			1, jctLines);

		// �J�������v�Z
		int w1 = pLSel->GetStringWidth(stationName);
		int w2 = pLSel->GetStringWidth(jctLines);
		if (width1 < w1) {
			width1 = w1;
		}
		if (width2 < w2) {
			width2 = w2;
		}
	}
	pLSel->SetColumnWidth(0, width1 + 16);
	pLSel->SetColumnWidth(1, width2 + 16);
}

//	���[�g�I���L�����Z��(���[�g�j���j�₢���킹
//
UINT Calps_mfcDlg::CancelRouteQuery()
{
	return MessageBox(_T("�o�H�I�𒆂ł������ׂăL�����Z�����Ă���낵���ł����H"),
										_T("�I���o�H�j���m�F"), MB_YESNO | MB_ICONQUESTION);
}

// "�w�肵�����w�����w�̌o�H�͎w��s�\�ł�"
//
void Calps_mfcDlg::CantSameStartAndEnd()
{
	MessageBox(_T("�w�肵�����w�����w�̌o�H�͎w��s�\�ł�"), _T("�w��s�����w"), MB_ICONEXCLAMATION);
}


//	�\���N���A
//	(��ʕ\���̏�����)
//
void Calps_mfcDlg::ResetContent()
{
	CListCtrl* pLSel = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_LINESTATIONS));

	m_route.removeAll();

	m_selMode = SEL_LINE;
	m_curStationId = 0;

	LVCOLUMN column;
	memset(&column, 0, sizeof(LVCOLUMN));

	column.mask = LVCF_TEXT | LVCF_WIDTH;

	column.pszText = _T("�H��");
	column.cx = 80;
	pLSel->SetColumn(0, &column);

	column.pszText = _T("");
	column.cx = 200;
	pLSel->SetColumn(1, &column);

	pLSel->DeleteAllItems();			// all record clear

	reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE))->DeleteAllItems();

	GetDlgItem(IDC_LIST_LINESTATIONS)->EnableWindow(FALSE);	// �w/�H�� �I�����X�g
	GetDlgItem(IDC_BUTTON_SEL)->EnableWindow(FALSE);		// [+]button

	GetDlgItem(IDC_BUTTON_BS)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_STAT)->SetWindowText(_T("���w�I�����Ă�������."));

	GetDlgItem(IDC_EDIT_START)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_END)->SetWindowText(_T(""));
	GetDlgItem(IDC_STATIC_LIST_SEL)->SetWindowText(_T("")); // ListView title
	GetDlgItem(IDC_RADIO_BRANCH_SEL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO_TERMINAL_SEL)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_BUTTON_STARTSEL)->SetFocus();

	SetDlgItemText(IDC_EDIT_RESULT, _T(""));

	resetMenu();

	GetDlgItem(IDC_BUTTON_REVERSE)->EnableWindow(FALSE);/* Disable [Reverse]�{�^�� */
	GetDlgItem(IDC_BUTTON_AUTOROUTE)->EnableWindow(FALSE);/* Disable [AutoRoute]�{�^�� */
	GetDlgItem(IDC_BUTTON_NEEREST)->EnableWindow(FALSE);/* Disable [Neerest]�{�^�� */

	//m_fareOptionMenuButton.DrawMenuBar();
}


//	���j���[���Z�b�g
//
void Calps_mfcDlg::resetMenu()
{
	/* [����K�p] */
	m_menu.CheckMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_CHECKED);
	m_menu.EnableMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_BYCOMMAND | MF_GRAYED);

	/* [����D�Ҍ��g�p] */
	m_menu.CheckMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_UNCHECKED);
	m_menu.EnableMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_BYCOMMAND | MF_GRAYED);

	m_menu.ModifyMenu(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_STRING, IDR_MENU_TERM_AGGR, _T("���w��P�w�Ɏw��"));
	m_menu.EnableMenuItem(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_GRAYED);

	m_menu.ModifyMenu(IDR_MENU_ARROUND_OSAKAKAN, MF_BYCOMMAND | MF_STRING, IDR_MENU_ARROUND_OSAKAKAN, _T("����������"));
	m_menu.EnableMenuItem(IDR_MENU_ARROUND_OSAKAKAN, MF_BYCOMMAND | MF_GRAYED);
}


//	[���o�[�X]Push�{�^��
//	�o�H���t�]����
//
void Calps_mfcDlg::OnBnClickedButtonReverse()
{
	int rc;

	//endStationId = m_route.endStationId();

	if (m_route.routeList().size() <= 1) {
		return;
	}

	rc = m_route.reverse();

	if (0 < rc) {
		GetDlgItem(IDC_EDIT_START)->SetWindowText(RouteUtil::StationName(m_route.startStationId()).c_str());	// ���w�\��
		UpdateRouteList();
	} else if (rc < 0) {
		AfxMessageBox(_T("�o�H���d�����Ă��܂�"));
	}
}

//	[�ŒZ�o�H]
//	���łɔ��w�A���w���w�肵�Ă���o�H���ŒZ�o�H�ōč\������
//
void Calps_mfcDlg::OnBnClickedButtonNeerest()
{
	int rc;

	if (m_route.routeList().size() <= 1) {
		return;
	}

	if (m_route.startStationId() == m_route.endStationId()) {
		MessageBox(_T("�J�n�w=�I���w�ł͍ŒZ�o�H�͎Z�o���܂���."),
			_T("�m�F"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	Route route;
	RouteList route_backup(m_route);
	rc = route.add(m_route.startStationId());
	ASSERT(rc == 1);
	rc = route.changeNeerest(false, m_route.endStationId());
	if ((rc == 5) || (rc == 0) || (rc == 4)) {
		SetDlgItemText(IDC_EDIT_STAT, _T("�o�H�͕Г������ɒB���Ă��܂�."));
		if (m_route.isModified()) {
			UpdateRouteList();
		}
		return;	/* already finished */
	}
	if (0 < rc) {

		m_route.assign(route);

		int numList = UpdateRouteList();	/* IDC_LIST_ROUTE update view */
		if (0 < numList) {
			return;				/* success */
		} else {
			ASSERT(FALSE);
		}
	} else if (-100 < rc) {
		MessageBox(_T("�o�H���d�����Ă��邽�ߎZ�o�ł��܂���ł���."),
				   _T("�������[�g"), MB_OK | MB_ICONEXCLAMATION);
		m_route.assign(route_backup);
		UpdateRouteList();	/* IDC_LIST_ROUTE update view */
	} else { /* < -1000  or 0(loop end. never) */
		MessageBox(_T("�Z�o�ł��܂���ł���."),
				   _T("�m�F"), MB_OK | MB_ICONEXCLAMATION);
		m_route.assign(route_backup);
		UpdateRouteList();	/* IDC_LIST_ROUTE update view */
	}
}


// [�o�H�L�^]
//
void Calps_mfcDlg::OnBnClickedButtonRoutecopy()
{
	tstring s = m_route.route_script();

	if (s.empty() || (0 == s.compare(m_lastRouteString))) {
		return;
	}

	s += _T("\n\n");

	CStdioFile file;

	if (file.Open(_T("route.txt"), CFile::modeCreate |CFile::modeReadWrite | CFile::modeNoTruncate)) {
		file.SeekToEnd();
		file.WriteString(s.c_str());
		m_lastRouteString.assign(s);
	} else {
		AfxMessageBox(_T("�t�@�C���I�[�v���G���["));
	}
}

//	[�o�H�J��]
//
void Calps_mfcDlg::OnBnClickedButtonRouteOpen()
{
	CString s;
	CFileDialog dlg(TRUE);
	HINSTANCE hInst =
	ShellExecute(NULL, _T("open"), _T("route.txt"), NULL, NULL, SW_SHOW);

	if (hInst <= (HINSTANCE)32) {
		s.Format(_T("�o�H�L�^�t�@�C���̃I�[�v���Ɏ��s���܂���. %d"), hInst);
		AfxMessageBox(s);

		if (IDYES == dlg.DoModal()) {
			ShellExecute(NULL, _T("open"), dlg.GetFileName(), NULL, NULL, SW_SHOW);
		}
	}
}

// [�o�H����]
//
void Calps_mfcDlg::OnBnClickedButtonRoutein()
{
	CRouteInputDlg dlg;
	if (IDOK == dlg.DoModal()) {
		parseAndSetupRoute(dlg.routeString);
	}
}

// [���ʋL�^]
//
void Calps_mfcDlg::OnBnClickedButtonResultcopy()
{
	tstring s;
	tstring result_string;
	CStdioFile file;

	CalcRoute croute(m_route);
	result_string = croute.showFare().c_str();

	int rp;
	do {
		rp = result_string.find(_T("\r\n"));
		if (rp == string::npos) {
			break;
		}
		result_string.replace(rp, 2, _T("\n"));
	} while (true);

	if (result_string.empty() || (0 == result_string.compare(m_lastResultString))) {
		return;
	}

	s = result_string + _T("\n------------------------------------------------\n");
	if (file.Open(_T("result.txt"), CFile::modeCreate |CFile::modeReadWrite | CFile::modeNoTruncate)) {
		file.SeekToEnd();
		file.WriteString(s.c_str());
		m_lastResultString.assign(result_string);
	} else {
		AfxMessageBox(_T("�t�@�C���I�[�v���G���["));
	}
}

//	m_route�̓��e��IDC_LIST_ROUTE���쐬���Ȃ���
//	@return IDC_LIST_ROUTE�̍s����Ԃ�
//
int Calps_mfcDlg::UpdateRouteList()
{
	int idx;
	int w0;
	int w1;
	int lineId = 0;
	int stationId = 0;

	CListCtrl* pLRoute = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE));

	pLRoute->DeleteAllItems();

	idx = m_route.routeList().size();
	if (idx <= 1) {

		GetDlgItem(IDC_BUTTON_BS)->EnableWindow(FALSE);		// Disnable [-] button
		return 0;
	}

	vector<RouteItem>::const_iterator pos = m_route.routeList().cbegin();
	w0 = w1 = 0;
	ASSERT((pos->lineId == 0) && (pos->stationId == m_route.startStationId()));
	pos++;
	for (idx = 0; pos != m_route.routeList().cend() ; pos++, idx++) {
		CString lineName;
		CString stationName;
		/*
		 * retrive line and station
		 *
		 */
		stationId = pos->stationId;
		stationName = RouteUtil::StationName(stationId).c_str();
		lineId = pos->lineId;
		lineName = RouteUtil::LineName(lineId).c_str();

		pLRoute->SetItemText(
			pLRoute->InsertItem(LVIF_TEXT | LVIF_PARAM,
								idx,
								lineName, 0, 0, 0,
								MAKEPAIR(lineId, stationId)),
								1, stationName);

		// �J�������v�Z
		int wn0 = pLRoute->GetStringWidth(lineName) + 16;
		int wn1 = pLRoute->GetStringWidth(stationName) + 16;
		if (w0 < wn0) {
			w0 = wn0;
		}
		if (w1 < wn1) {
			w1 = wn1;
		}
	}
	ASSERT(pLRoute->GetItemCount() == idx);

	if (0 < idx) {
		// �J�������ݒ�
		if (pLRoute->GetColumnWidth(0) < w0) {
			pLRoute->SetColumnWidth(0, w0);
		}
		if (pLRoute->GetColumnWidth(1) < w1) {
			pLRoute->SetColumnWidth(1, w1);
		}

		// UI mode set
		m_selMode = SEL_LINE;				/* for [-] button */
		setupForLinelistByStation(stationId, lineId);	// ���w�̏����H��

		m_curStationId = stationId;

		GetDlgItem(IDC_BUTTON_BS)->EnableWindow(TRUE);			// Enable [-] button

		/* �^���\�� */
		showFare();
		GetDlgItem(IDC_EDIT_END)->SetWindowText(RouteUtil::StationName((m_route.routeList().cend() - 1)->stationId).c_str());	// ���w�\��
	} else {
		GetDlgItem(IDC_BUTTON_BS)->EnableWindow(FALSE);		// Disnable [-] button
	}
	UpdateWindow();
	return idx;
}



/*	Dialog drag and drop
 *  �t�@�C����D&D���邱�ƂŌo�H��ǉ�����@�\
 *	�t�@�C���͒P��e�L�X�g�t�@�C���݂̂�1�s��1�s�̂ݓǂݍ���Őݒ�
 *	1�s�݂̂�, ',', ' ', '|', '/', (tab)�Ȃǂ̋�؂��"�w�A�H���A����w�A�H���A..."�̕���
 *
 */
void Calps_mfcDlg::OnDropFiles(HDROP hDropInfo)
{
	TCHAR path[MAX_PATH];
    CString str;

    if (1 != DragQueryFile(hDropInfo, 0xffffffff, NULL, 0)) {
		return;	/* ignore multi file drop */
	}

	DragQueryFile(hDropInfo, 0, path, MAX_PATH);
	try {
		CStdioFile fn(path, CFile::modeRead);
		fn.ReadString(str);
		parseAndSetupRoute(str);
	} catch (CFileException *e) {
		e->Delete();
	}
}


/*	�^���\��
 *
 */
void Calps_mfcDlg::showFare(bool bResetOption/* = true */)
{
	int16_t opt;
	int16_t opt_chk;

	if (bResetOption) {
		//	[����K�p]�`�F�b�N�{�^��
		//	[JR���C����D�Ҍ��g�p]�`�F�b�N�{�^��
		//	[���w��P�w�Ɏw��^���w��P�w�Ɏw��]
		m_route.setFareOption(
			FAREOPT_RULE_APPLIED | FAREOPT_APPLIED_TERMINAL | FAREOPT_JRTOKAI_STOCK_NO_APPLIED,
			FAREOPT_AVAIL_RULE_APPLIED | FAREOPT_AVAIL_APPLIED_START_TERMINAL | FAREOPT_AVAIL_APPLIED_JRTOKAI_STOCK);
		resetMenu();
	}
	/*	�^���\�������t���O�擾 */
	CalcRoute croute(m_route);
	SetDlgItemText(IDC_EDIT_RESULT, croute.showFare().c_str());

	opt = croute.getFareOption();

	if ((opt & 0xc0) != 0) {
		/* not enough route */
		GetDlgItem(IDC_BUTTON_REVERSE)->EnableWindow(FALSE);/* Disable [Reverse]�{�^�� */
		m_menu.CheckMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_UNCHECKED); /* [����K�p]Uncheck��� */
		GetDlgItem(IDC_BUTTON_NEEREST)->EnableWindow(FALSE);/* Disable [Neerest]�{�^�� */
	}
	else {
		GetDlgItem(IDC_BUTTON_REVERSE)->EnableWindow(TRUE); /* Enable [Reverse]�{�^�� */
		GetDlgItem(IDC_BUTTON_NEEREST)->EnableWindow(TRUE);/* Disable [Neerest]�{�^�� */
	}

	// ����K�p
	if ((opt_chk = (opt & 0x30)) != 0) {
		m_menu.EnableMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_BYCOMMAND | MF_ENABLED);
		if (opt_chk == 0x20) {
			// �����K�p
			m_menu.CheckMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_UNCHECKED);
			m_menu.EnableMenuItem(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_GRAYED);
		}
		else {
			// ����K�p
			m_menu.CheckMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_CHECKED);


			// �卂 ��� ���{�� ���w�E���w��P�w�Ɏw��
			switch (opt & 0x03) {
			case 0:
				m_menu.EnableMenuItem(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_GRAYED);
				break;

			case 1:		// �u���w��P�w�Ɏw��v
				m_menu.ModifyMenu(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_STRING, IDR_MENU_TERM_AGGR, _T("���w��P�w�Ɏw��"));
				m_menu.EnableMenuItem(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_ENABLED);
				break;
			case 2:		// �u���w��P�w�Ɏw��v
				m_menu.ModifyMenu(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_STRING, IDR_MENU_TERM_AGGR, _T("���w��P�w�Ɏw��"));
				m_menu.EnableMenuItem(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_ENABLED);
				break;
			default:
				ASSERT(FALSE);
				break;
			}
		}
	}
	else {
		// ���� ���֌W
		m_menu.EnableMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_BYCOMMAND | MF_GRAYED);
		m_menu.EnableMenuItem(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_GRAYED);
	}

	// JR���C����D�Ҍ��g�p
	if ((opt_chk = (opt & 0x300)) != 0) {
		m_menu.EnableMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_BYCOMMAND | MF_ENABLED);
		if (opt_chk == 0x200) {
			m_menu.CheckMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_CHECKED);
		}
		else {
			m_menu.CheckMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_UNCHECKED);
		}
	}
	else {
		// JR���C����D�Ҍ��g�p �I�����Ȃ�(���֌W)
		m_menu.CheckMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_UNCHECKED);
		m_menu.EnableMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_BYCOMMAND | MF_GRAYED);
	}

	// ����� �����^�O���
	switch ((opt >> 2) & 0x03) {
	case 0:
		m_menu.EnableMenuItem(IDR_MENU_ARROUND_OSAKAKAN, MF_BYCOMMAND | MF_GRAYED);
		break;
	case 1:
		m_menu.EnableMenuItem(IDR_MENU_ARROUND_OSAKAKAN, MF_BYCOMMAND | MF_ENABLED);
		m_menu.ModifyMenu(IDR_MENU_ARROUND_OSAKAKAN, MF_BYCOMMAND | MF_STRING, IDR_MENU_ARROUND_OSAKAKAN, _T("����������"));
		break;
	case 2:
		m_menu.EnableMenuItem(IDR_MENU_ARROUND_OSAKAKAN, MF_BYCOMMAND | MF_ENABLED);
		m_menu.ModifyMenu(IDR_MENU_ARROUND_OSAKAKAN, MF_BYCOMMAND | MF_STRING, IDR_MENU_ARROUND_OSAKAKAN, _T("������߉��"));
		break;
	default:
		ASSERT(FALSE);
		break;
	}
}


//	Dialog Double click
//
//
void Calps_mfcDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
#ifdef _DEBUG		//	for TEST
	//int numList;

	//CListCtrl* pLRoute = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE));

	//test_exec();	// �P�̃e�X�g�����s(���ʂ�test_result.txt)
	//numList = UpdateRouteList();	/* IDC_LIST_ROUTE update view */

	//m_curStationId = IDENT2(pLRoute->GetItemData(numList - 1));	// last station
	//m_selMode = SEL_LINE;				/* for [-] button */
	//int curLineId = IDENT1(pLRoute->GetItemData(numList - 1));	// last line
	//ASSERT(0 < curLineId);
	//setupForLinelistByStation(m_curStationId, curLineId);	// ���w�̏����H��

	if (0 < m_route.routeList().size()) {

		vector<RouteItem>::const_iterator pos = m_route.routeList().cbegin();
		TRACE(_T("\nbegin befor: %s\n"), RouteUtil::StationName(pos->stationId).c_str());
		for (++pos; pos != m_route.routeList().cend(); pos++) {
			TRACE(_T("%s, %s, %d\n"), RouteUtil::LineName(pos->lineId).c_str(), RouteUtil::StationName(pos->stationId).c_str(), pos->flag>>31);
		}
		TRACE(_T("\n"));

UpdateRouteList();

pos = m_route.routeList().cbegin();
TRACE(_T("\nbegin after: %s\n"), RouteUtil::StationName(pos->stationId).c_str());
for (++pos; pos != m_route.routeList().cend(); pos++) {
	TRACE(_T("%s, %s, %d\n"), RouteUtil::LineName(pos->lineId).c_str(), RouteUtil::StationName(pos->stationId).c_str(), pos->flag >> 31);
}
TRACE(_T("\n"));
	}
 else {
	 GetDlgItem(IDC_BUTTON_BS)->EnableWindow(FALSE);		// Disnable [-] button
 }
#endif
 CRouteInputDlg dlg;
 if (IDOK == dlg.DoModal()) {
	 parseAndSetupRoute(dlg.routeString);
 }
}


// ������Ŏw�肵���o�H��ݒ肷��
//
int Calps_mfcDlg::parseAndSetupRoute(LPCTSTR route_str)
{
	int rc;

	ResetContent();

	rc = m_route.setup_route(route_str);

	GetDlgItem(IDC_EDIT_START)->SetWindowText(RouteUtil::StationName(m_route.startStationId()).c_str());	// ���w�\��

	switch (rc) {
	case -200:
		AfxMessageBox(_T("�w���s�����܂܂�Ă��܂�"));
		break;
	case -300:
		AfxMessageBox(_T("�����s�����܂܂�Ă��܂�"));
		break;
	case -100:
		AfxMessageBox(_T("�v���I�G���["));
		break;
	case -1:
		AfxMessageBox(_T("�o�H���d�����Ă��܂�"));
		break;
	case -2:
		AfxMessageBox(_T("�s���Ȍo�H�w��ł�"));
		break;
	case -4:
		AfxMessageBox(_T("������Ă��Ȃ���А��ʉ߂ł�"));
		break;
	case 4:	/* ��А��ʉߏI�[ */
	case 0:
	case 1:
		/* success */
		break;
	default:
		AfxMessageBox(_T("�s���ȃG���["));
		break;
	}
	if (0 <= rc) {
		UpdateRouteList();
		GetDlgItem(IDC_LIST_LINESTATIONS)->EnableWindow(TRUE);	// �w/�H�� �I�����X�g�I����
		GetDlgItem(IDC_BUTTON_SEL)->EnableWindow(TRUE);			// [+] button
		GetDlgItem(IDC_BUTTON_AUTOROUTE)->EnableWindow(TRUE);	/* Enable [AutoRoute]�{�^�� */
	}
	else {
		ResetContent();
	}
	return 0;
}

//	[���ʂ��J��]
//
void Calps_mfcDlg::OnBnClickedButtonRsltopen()
{
	CString s;
	CFileDialog dlg(TRUE);
	HINSTANCE hInst =
		ShellExecute(NULL, _T("open"), _T("result.txt"), NULL, NULL, SW_SHOW);

	if (hInst <= (HINSTANCE)32) {
		s.Format(_T("���ʋL�^�t�@�C���̃I�[�v���Ɏ��s���܂���. %d"), hInst);
		AfxMessageBox(s);

		if (IDYES == dlg.DoModal()) {
			ShellExecute(NULL, _T("open"), dlg.GetFileName(), NULL, NULL, SW_SHOW);
		}
	}
}



void CAboutDlg::OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult)
{
	ShellExecuteW(m_hWnd, L"open", L"http://farert.blogspot.jp", NULL, NULL, SW_SHOWNORMAL);
	*pResult = 0;
}

//	[�v�Z�I�v�V����...] MenuBarButton
//
void Calps_mfcDlg::OnBnClickedMfcmenubuttonFareopt()
{
	UINT state;

	switch (m_fareOptionMenuButton.m_nMenuResult) {
	case IDR_MENU_KOKURA_HAKATA_BULLET_NOTSAME:
		//	[����-���q �V�����ݗ����ʐ�����]�`�F�b�N�{�^��
		state = m_menu.GetMenuState(IDR_MENU_KOKURA_HAKATA_BULLET_NOTSAME, MF_BYCOMMAND);
		if (MF_CHECKED & state) {
			m_menu.CheckMenuItem(IDR_MENU_KOKURA_HAKATA_BULLET_NOTSAME, MF_UNCHECKED);
			m_route.setNotSameKokuraHakataShinZai(false);
		}
		else {
			m_menu.CheckMenuItem(IDR_MENU_KOKURA_HAKATA_BULLET_NOTSAME, MF_CHECKED);
			m_route.setNotSameKokuraHakataShinZai(true);
			MessageBox(_T("����-���q �V�����ݗ����ʐ�������I�����܂���\r\n�Y���o�H�͑I�����Ȃ����K�v������܂�"), _T("�v�Z�I�v�V����"), MB_ICONINFORMATION);
		}
		break;

	case IDR_MENU_ARROUND_OSAKAKAN:
		//	[����������^������߉��]
		{
			/* �����1�񂾂��ʂ��Ă���? */
			int opt = 0x03 & (m_route.getFareOption() >> 2);
			int rc;

			ASSERT(opt == 1 || opt == 2);
			if ((opt == 1) || (opt == 2)) {
				// �߉�莞�ɉ����ꂽ�牓���(FAREOPT_OSAKAKAN_DETOUR)�� :
				// ����莞�ɉ����ꂽ��߉��(0:FAREOPT_OSAKAKAN_SHORTCUT)��
				rc = m_route.setDetour((opt == 1) ? true : false);
				if (1 == rc) {
					showFare(false);
					// opt�͋t�]����
					// �܂��͖����ƂȂ肱�̃{�^���͉����Ȃ��Ȃ�
				}
				else if (rc != 0) {
					AfxMessageBox(_T("�o�H���d�����Ă��܂�"));
				}
				else {
					// rc == 0
					int selId = m_route.routeList().back().stationId;

					MessageBox(_T("�o�H���Г������ɒB���܂���. "), _T("�o�H�I�["), MB_ICONQUESTION);

					CListCtrl* pLRoute = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE));
					int numList = pLRoute->GetItemCount();

					// 2��ڂ͊��ɋ󗓂ƂȂ��Ă���̂ŁBASSERT(selId == IDENT2(pLRoute->GetItemData(numList - 1)));	// last station
					ASSERT(m_route.routeList().back().lineId == IDENT1(pLRoute->GetItemData(numList - 1)));	// last line

					pLRoute->SetItemText(numList - 1, 1, _T(""));
					pLRoute->SetItemData(numList - 1, MAKEPAIR(m_route.routeList().back().lineId, 0));

					setupForLinelistByStation(selId, m_route.routeList().back().lineId);
					showFare(false);
				}
			}
			else {
				/* showFare()�̌�ł����Ȃ��Ă��邽�ߕ��ʂ͕s�v(���S��)*/
				//!		GetDlgItem(IDC_BUTTON_OSAKAKAN)->EnableWindow(FALSE);
			}
		}
		break;

	case IDR_MENU_SPECIFIC_APPLY:
		//	[����K�p]�`�F�b�N�{�^��
		state = m_menu.GetMenuState(IDR_MENU_SPECIFIC_APPLY, MF_BYCOMMAND);
		if (MF_CHECKED & state) {
			m_menu.CheckMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_UNCHECKED);
			m_route.setFareOption(FAREOPT_RULE_NO_APPLIED, FAREOPT_AVAIL_RULE_APPLIED);
		}
		else {
			m_menu.CheckMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_CHECKED);
			m_route.setFareOption(FAREOPT_RULE_APPLIED, FAREOPT_AVAIL_RULE_APPLIED);
		}
		showFare(false);
		break;

	case IDR_MENU_TERM_AGGR:
		//	[���w��P�w�Ɏw��^���w��P�w�Ɏw��]
		{
			CalcRoute croute(m_route);
			uint32_t opt = 3 & croute.getFareOption();
			if (opt == 2) {
				m_route.setFareOption(FAREOPT_APPLIED_START, FAREOPT_AVAIL_APPLIED_START_TERMINAL);
			}
			else if (opt == 1) {
				m_route.setFareOption(FAREOPT_APPLIED_TERMINAL, FAREOPT_AVAIL_APPLIED_START_TERMINAL);
			}
			showFare(false);
		}
		break;

	case IDR_MENU_JRTOKAI_STOCK:
		//	[JR���C����D�Ҍ��g�p]�`�F�b�N�{�^��
		state = m_menu.GetMenuState(IDR_MENU_JRTOKAI_STOCK, MF_BYCOMMAND);
		if (MF_CHECKED & state) {
			m_menu.CheckMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_UNCHECKED);
			m_route.setFareOption(FAREOPT_JRTOKAI_STOCK_NO_APPLIED, FAREOPT_AVAIL_APPLIED_JRTOKAI_STOCK);
		}
		else {
			m_menu.CheckMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_CHECKED);
			m_route.setFareOption(FAREOPT_JRTOKAI_STOCK_APPLIED, FAREOPT_AVAIL_APPLIED_JRTOKAI_STOCK);
		}
		showFare(false);
		break;

	default:
		ASSERT(FALSE);
		break;
	}
}


void Calps_mfcDlg::OnStnClickedStaticBar()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
}
