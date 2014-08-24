
// alps_mfcDlg.cpp : �����t�@�C��
//
//	Copyright(c) sutezo9@me.com 2012 allrightreserved.
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
#define isComplete()	((0 < m_route.routeList().size()) && \
						 (m_route.endStationId() == m_curStationId))



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
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
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
}

BEGIN_MESSAGE_MAP(Calps_mfcDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_STARTSEL, &Calps_mfcDlg::OnBnClickedButtonStartsel)
	ON_BN_CLICKED(IDC_BUTTON_ENDSEL, &Calps_mfcDlg::OnBnClickedButtonEndsel)
	ON_BN_CLICKED(IDC_BUTTON_SEL, &Calps_mfcDlg::OnBnClickedButtonSel)
	ON_BN_CLICKED(IDC_BUTTON_BS, &Calps_mfcDlg::OnBnClickedButtonBs)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LINESTATIONS, &Calps_mfcDlg::OnNMDblclkListStations)
	ON_BN_CLICKED(IDC_RADIO_BRANCH_SEL, &Calps_mfcDlg::OnBnClickedRadioBranchSel)
	ON_BN_CLICKED(IDC_RADIO_TERMINAL_SEL, &Calps_mfcDlg::OnBnClickedRadioTerminalSel)
	ON_BN_CLICKED(IDC_BUTTON_ALL_CLEAR, &Calps_mfcDlg::OnBnClickedButtonAllClear)
	ON_BN_CLICKED(IDC_BUTTON_TERM_CLEAR, &Calps_mfcDlg::OnBnClickedButtonTermClear)
	ON_BN_CLICKED(IDC_BUTTON_AUTOROUTE, &Calps_mfcDlg::OnBnClickedButtonAutoroute)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_CHECK_RULEAPPLY, &Calps_mfcDlg::OnBnClickedCheckRuleapply)
	ON_BN_CLICKED(IDC_BUTTON_REVERSE, &Calps_mfcDlg::OnBnClickedButtonReverse)
	ON_BN_CLICKED(IDC_BUTTON_ROUTECOPY, &Calps_mfcDlg::OnBnClickedButtonRoutecopy)
	ON_BN_CLICKED(IDC_BUTTON_ROUTEIN, &Calps_mfcDlg::OnBnClickedButtonRoutein)
	ON_BN_CLICKED(IDC_BUTTON_RESULTCOPY, &Calps_mfcDlg::OnBnClickedButtonResultcopy)
	ON_BN_CLICKED(IDC_BUTTON_RSLTOPEN, &Calps_mfcDlg::OnBnClickedButtonRsltopen)
	ON_BN_CLICKED(IDC_BUTTON_ROUTE_OPEN, &Calps_mfcDlg::OnBnClickedButtonRouteOpen)
	ON_BN_CLICKED(IDC_BUTTON_SPECIAL_CITY, &Calps_mfcDlg::OnBnClickedButtonSpecialCity)
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

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	//--
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
			// �J�n�w=�I���w�͖Ӓ����łȂ����Ƃ��m�F
			if (m_route.endStationId()  == startStationId) {
				if (Route::NumOfNeerNode(startStationId) <= 1) {
					CantSameStartAndEnd();	// "�w�肵�����w�����w�̌o�H�͎w��s�\�ł�"
					return;
				}
			}
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
		}// �s�ύX�Ȃ�Ȃɂ����Ȃ�
	}
}

// ���w�w��
//	IDC_BUTTON_ENDSEL [...] button pushed.
//
void Calps_mfcDlg::OnBnClickedButtonEndsel()
{
	int numList;
	int endStationId;
	CListCtrl* pLRoute = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE));

	CTermSel dlg(true, this);

	if (IDOK != dlg.DoModal()) {
		return;
	}
	endStationId = dlg.getStationId();
	if (endStationId == m_route.endStationId()) {
		return;
	}
	ASSERT(0 < endStationId);

	// �J�n�w=�I���w�͖Ӓ����łȂ����Ƃ��m�F
	if (endStationId == m_route.startStationId()) {
		if (Route::NumOfNeerNode(endStationId) <= 1) {
			CantSameStartAndEnd();	// "�w�肵�����w�����w�̌o�H�͎w��s�\�ł�"
			return;
		}
	}

	/* �J�n�w���I���w�ŁA�I���w�����Ɍo�H��ɒʉ߂��Ă��邩�`�F�b�N */
	if ((endStationId != m_route.startStationId()) && 
		m_route.checkPassStation(endStationId)) {
		if (isComplete() || (IDYES == MessageBox(
_T("���w�͌o�H�������ɒʉ߂��Ă���܂�. ���w�ȍ~�̌o�H���L�����Z�����܂����H"), 
_T("���w�}���A�o�H�Z�k"), 
			MB_ICONEXCLAMATION|MB_YESNO|MB_DEFBUTTON2))) {

			m_route.terminate(endStationId);	/* set end station */
			/* numList = */UpdateRouteList();	/* IDC_LIST_ROUTE update view */

			ASSERT(m_curStationId == endStationId);
			//m_selMode = SEL_LINE;				/* for [-] button */
			//int curLineId = IDENT1(pLRoute->GetItemData(numList - 1));	// last line
			//ASSERT(0 < curLineId);
			//setupForLinelistByStation(m_curStationId, curLineId);	// ���w�̏����H��
			routeEnd();
			///* �^���\�� */
			///showFare();
		} else {
			return;
		}
	} else { /* �I���w�͌o�H��܂��ʉ߂��Ă��Ȃ��ꍇ */
		numList = pLRoute->GetItemCount();
		if (isComplete()) {
			// ���ɕʂ̏I���w�Łu�����v���Ă���ꍇ�A���X�g�I�[���L�����Z����([-]�{�^�����l)
			// �I�����X�g���u(�I��)�w�̏����H���ꗗ�v->�u�H���̕���w�ꗗ�v�֕ύX
			m_route.removeTail();
			if (2 <= numList) {
				m_curStationId = IDENT2(pLRoute->GetItemData(numList - 2));
			} else {
				m_curStationId = m_route.startStationId();		// ���w
			}
			GetDlgItem(IDC_LIST_LINESTATIONS)->EnableWindow(TRUE);	// �w/�H�� �I�����X�g
			GetDlgItem(IDC_BUTTON_SEL)->EnableWindow(TRUE);			// [+] button
			m_selMode = SEL_JUNCTION;
		}
		m_route.setEndStationId(endStationId);	/* �I���w�X�V */
		if (m_selMode != SEL_LINE) {
			// �I�����X�g���u�H���̕���w�ꗗ�vor �u�H���̉w�ꗗ�v�\�����������ꍇ
			// �u(���w)�v�ƕ\�����ꂽ���X�g���X�V����ׁA�w�I�����X�g���X�V
			setupForStationlistByLine(IDENT1(pLRoute->GetItemData(numList - 1)), m_curStationId);
		}
	}
	SetDlgItemText(IDC_EDIT_END, dlg.getStationName());		// ���w�\��
	GetDlgItem(IDC_BUTTON_TERM_CLEAR)->EnableWindow(TRUE);
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
	} else {
		// add route list �wId

		/* �o�H�ǉ� */// last line
		curLineId = IDENT1(pLRoute->GetItemData(nRoute - 1));	//(stationId is not yet strage in HIWORD)
		int rslt = m_route.add(curLineId, /*m_curStationId,*/selId);
		if (m_route.isModified()) {
			if (0 < ModifyRouteList()) {		// �o�H�\���X�V
				curLineId = m_route.routeList().back().lineId;
				selId = m_route.routeList().back().stationId;
			} else {
				ResetContent();
				return;
			}
		}
		if (rslt < 0) {
			ASSERT(rslt == -1);		/* -1 �ȊO�̓o�O(DB�G���[���A�����s��) */
			if (-1 != rslt) {
				AfxMessageBox(_T("Fatal error occured."));
				ResetContent();
				return;
			}
			SetDlgItemText(IDC_EDIT_STAT, _T("�o�H���d�����Ă��܂�."));
			return;
		}
		if (0 == rslt) {	/* fin */
			if (m_route.endStationId() != selId) {
				if ((m_selMode != SEL_TERMINATE) &&
				(IDYES != MessageBox(
				_T("�o�H���Г������ɒB���܂���. ���w�Ƃ��ďI�����܂����H"), _T("�o�H�I�["),
										MB_YESNO | MB_ICONQUESTION))) {
					m_route.removeTail();
					if (0 < ModifyRouteList()) {		// �o�H�\���X�V
						curLineId = m_route.routeList().back().lineId;
						selId = m_route.routeList().back().stationId;
					} else {
						ResetContent();
					}
					return;
				}
				m_route.end();
				m_route.setEndStationId(selId);				// ���w�ύX
				SetDlgItemText(IDC_EDIT_END, selTitle);
				GetDlgItem(IDC_BUTTON_TERM_CLEAR)->EnableWindow(TRUE);
			}
			setupForLinelistByStation(selId, curLineId);
			routeEnd();
		} else if (1 != rslt) {	// undefine
			ASSERT(FALSE);
			return;
		} else { 				/* rslt == 1 */
			setupForLinelistByStation(selId, curLineId);

			if ((m_selMode == SEL_TERMINATE) || (selId == m_route.endStationId())) {
				/* ���w�w�� */
				if (selId != m_route.endStationId()) {
					m_route.setEndStationId(selId);		/* ���w:������������ */
					SetDlgItemText(IDC_EDIT_END, selTitle);						// ���w�\��
					GetDlgItem(IDC_BUTTON_TERM_CLEAR)->EnableWindow(TRUE);		// 
				}
				routeEnd();
			} else if ((rslt == 1) && !m_route.isModified()) {	/* OK */
				pLRoute->SetItemText(nRoute - 1, 1, selTitle);					// �o�H�F����w�\��
				pLRoute->SetItemData(nRoute - 1, MAKEPAIR(curLineId, selId));	//�o�H�F����wID�ݒ�

				// �J����������
				wc = pLRoute->GetColumnWidth(1);
				wn = pLRoute->GetStringWidth(selTitle) + 16;
				if (wc < wn) {
					pLRoute->SetColumnWidth(1, wn);
				}
			}
		}
		
		/* �^���\�� */
		showFare();
		m_curStationId = selId;
		m_selMode = SEL_LINE;	/* for [-] button */
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

	if (isComplete()) {
		GetDlgItem(IDC_LIST_LINESTATIONS)->EnableWindow(TRUE);	// �w/�H�� �I�����X�g
		GetDlgItem(IDC_BUTTON_SEL)->EnableWindow(TRUE);			// [+] button
	}
	if (m_selMode == SEL_LINE) {
		/* �H�����X�g�\���� */
		ASSERT(2 <= m_route.routeList().size());
		ASSERT((m_curStationId == m_route.endStationId()) || (m_curStationId == IDENT2(pLRoute->GetItemData(numList - 1))));
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

//	���w�N���A
//	IDC_BUTTON_TERM_CLEAR [X] button pushed.
//
void Calps_mfcDlg::OnBnClickedButtonTermClear()
{
	CListCtrl* pLRoute = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE));
	int numList = pLRoute->GetItemCount();

	if (isComplete()) {
		// �u�����v
		m_route.removeTail();
		if (2 <= numList) {
			m_curStationId = IDENT2(pLRoute->GetItemData(numList - 2));
		} else {
			m_curStationId = m_route.startStationId();		// ���w
		}
		GetDlgItem(IDC_LIST_LINESTATIONS)->EnableWindow(TRUE);	// �w/�H�� �I�����X�g
		GetDlgItem(IDC_BUTTON_SEL)->EnableWindow(TRUE);			// [+] button
		m_selMode = SEL_JUNCTION;
	}
	m_route.setEndStationId(0);
	if (m_selMode != SEL_LINE) {
		// �u(���w)�v�ƕ\�����ꂽ���X�g���X�V����ׁA�w�I�����X�g���X�V
		setupForStationlistByLine(IDENT1(pLRoute->GetItemData(numList - 1)), m_curStationId);
	}
	SetDlgItemText(IDC_EDIT_END, _T(""));
	GetDlgItem(IDC_BUTTON_TERM_CLEAR)->EnableWindow(FALSE);
}

//	IDC_BUTTON_AUTOROUTE [�ŒZ�o�H] button pushed
//
void Calps_mfcDlg::OnBnClickedButtonAutoroute()
{
	int rc;
	
	if ((m_route.startStationId ()<= 0) || (m_route.endStationId() <= 0)) {
		MessageBox(_T("�J�n�w�A�I���w���w�肵�Ȃ��ƍŒZ�o�H�͎Z�o���܂���."),
										_T("�m�F"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	if (m_route.startStationId()== m_route.endStationId()) {
		MessageBox(_T("�J�n�w=�I���w�ł͍ŒZ�o�H�͎Z�o���܂���."),
										_T("�m�F"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	
	rc = m_route.changeNeerest((IDYES == MessageBox(_T("�V�������܂߂܂���?"),
										_T("�m�F"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2)));
	if (0 <= rc) {
		CListCtrl* pLRoute = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE));
		int numList = UpdateRouteList();	/* IDC_LIST_ROUTE update view */
		if (0 < numList) {
			ASSERT(m_curStationId == m_route.endStationId());
			//m_selMode = SEL_LINE;				/* for [-] button */
			//int curLineId = IDENT1(pLRoute->GetItemData(numList - 1));	// last line
			//ASSERT(0 < curLineId);
			//setupForLinelistByStation(m_curStationId, curLineId);	// ���w�̏����H��
			//GetDlgItem(IDC_BUTTON_BS)->EnableWindow(TRUE);			// Enable [-] button
			//
			///* �^���\�� */
			///showFare();
			routeEnd();
			return;				/* success */
		} else {
			ASSERT(FALSE);
		}
	} else if (-100 < rc) {
		MessageBox(_T("�o�H���d�����Ă��邽�ߎZ�o�ł��܂���ł���."),
				   _T("�������[�g"), MB_OK | MB_ICONEXCLAMATION);
	} else { /* < -1000 */
		MessageBox(_T("�Z�o�ł��܂���ł���."),
				   _T("�m�F"), MB_OK | MB_ICONEXCLAMATION);
	}
	if (m_route.isModified()) {
		if (0 < ModifyRouteList()) {		// �o�H�\���X�V
			int curLineId = m_route.routeList().back().lineId;
			int selId = m_route.routeList().back().stationId;
			setupForLinelistByStation(selId, curLineId);
			showFare();
			routeEnd();
		}
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
	DBO dbo = Route::Enum_line_of_stationId(stationId);
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
		dbo = Route::Enum_junction_of_lineId(lineId, m_route.endStationId());
	} else if (m_selMode == SEL_TERMINATE) {
		dbo = Route::Enum_station_of_lineId(lineId);
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
			(stationId == curStationId) ? _T("- ") : ((stationId == m_route.endStationId()) ? _T("(���w)") : _T("")), 
			dbo.getText(0).c_str());

		CString jctLines;
		if (0 != dbo.getInt(2)) { /* ����w */
			// ����w�̏�����H����'/'�ŋ�؂�񋓂�����������쐬
			DBO dbo_lines = Route::Enum_line_of_stationId(stationId);
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

	GetDlgItem(IDC_BUTTON_ENDSEL)->EnableWindow(TRUE); // ���w�I��
	GetDlgItem(IDC_BUTTON_STARTSEL)->EnableWindow(TRUE);

	GetDlgItem(IDC_BUTTON_TERM_CLEAR)->EnableWindow(FALSE);

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

	CheckDlgButton(IDC_CHECK_RULEAPPLY, BST_CHECKED);	/* [����K�p]�{�^����������� */
	GetDlgItem(IDC_BUTTON_SPECIAL_CITY)->EnableWindow(FALSE);
}

//	���w�܂ł̎w�芮��
//
//
void Calps_mfcDlg::routeEnd()
{
	/* �I���w�w�� */
	GetDlgItem(IDC_RADIO_BRANCH_SEL)->ShowWindow(SW_HIDE);		// RADIO BUTTON[����w]
	GetDlgItem(IDC_RADIO_TERMINAL_SEL)->ShowWindow(SW_HIDE);	// RADIO BUTTON[�I���w]
	SetDlgItemText(IDC_STATIC_LIST_SEL, _T(""));				// Select List view title
	GetDlgItem(IDC_LIST_LINESTATIONS)->EnableWindow(FALSE);		// [�H��/����w/���w]Select List View
	GetDlgItem(IDC_BUTTON_SEL)->EnableWindow(FALSE);			// [+]
	// ���w�̏����H���̕\���̖�ڂƂ��Ďc�����Ƃɂ��邽�߂ɃR���e���c�͏������Ȃ�
	//reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_LINESTATIONS))
	//->DeleteAllItems();											// all record clear
	SetDlgItemText(IDC_EDIT_STAT, _T("����"));
}



//	[���w��P�w�Ɏw��^���w��P�w�Ɏw��]
//
void Calps_mfcDlg::OnBnClickedButtonSpecialCity()
{
	/* �P�w�I���\�ȓ���s��s��������? */
	int opt = m_route.fareCalcOption();
	ASSERT(opt == 1 || opt == 2);
	if ((opt == 1) || (opt == 2)) {
		/* �P�w�w�� */
		CString cap;
		GetDlgItem(IDC_BUTTON_SPECIAL_CITY)->GetWindowText(cap);

		if (0 <= cap.Find(_T("���w"))) { // ���w=�s��s��
			SetDlgItemText(IDC_BUTTON_SPECIAL_CITY, _T("���w��P�w�Ɏw��"));
		} else {		// ���w=�s��s��
			SetDlgItemText(IDC_BUTTON_SPECIAL_CITY, _T("���w��P�w�Ɏw��"));
		}
	}
	showFare();
}

//	[����K�p]�`�F�b�N�{�^��
//
void Calps_mfcDlg::OnBnClickedCheckRuleapply()
{
	showFare();
}

//	[���o�[�X]Push�{�^��
//	�o�H���t�]����
//
void Calps_mfcDlg::OnBnClickedButtonReverse()
{
	int rc;
	int endStationId;

	endStationId = m_route.endStationId();

	if (m_route.routeList().size() <= 1) {
		if ((0 < m_route.endStationId()) && (0 < m_route.startStationId())) {
			m_route.setEndStationId(m_route.startStationId());
			m_curStationId = endStationId;
			m_route.add(endStationId);
			GetDlgItem(IDC_EDIT_START)->SetWindowText(Route::StationName(m_route.startStationId()).c_str());	// ���w�\��
			GetDlgItem(IDC_EDIT_END)->SetWindowText(Route::StationName(m_route.endStationId()).c_str());	// ���w�\��

			setupForLinelistByStation(m_curStationId);				// ���w�̏����H���ꗗ�̕\��
		}
		return;
	}
	
	rc = m_route.reverse();

	if (0 < rc) {
		GetDlgItem(IDC_EDIT_START)->SetWindowText(Route::StationName(m_route.startStationId()).c_str());	// ���w�\��
		UpdateRouteList();
		if (0 < endStationId) {
			if (endStationId == m_route.startStationId()) {
				endStationId = (m_route.routeList().cend() - 1)->stationId;
			}
			m_route.setEndStationId(endStationId);
			GetDlgItem(IDC_EDIT_END)->SetWindowText(Route::StationName(endStationId).c_str());	// ���w�\��
		}
	} else if (rc < 0) {
		AfxMessageBox(_T("�o�H���d�����Ă��܂�"));
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
	int flag;

	flag = 0;

	CString cap;
	GetDlgItem(IDC_BUTTON_SPECIAL_CITY)->GetWindowText(cap);

	/* �P�w�I���\�ȓ���s��s��������? */
	if (0 <= cap.Find(_T("���w"))) {
		flag = APPLIED_START;	// ���w��s��s����(�K�p�́u���{��-�卂�v�ޗ�̂�)
	}
	/* �K���K�p��? */
	flag |=	((BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_RULEAPPLY)) ? RULE_APPLIED : RULE_NO_APPLIED);
	result_string = m_route.showFare(flag).c_str();
	
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

	if (m_route.routeList().size() <= 1) {
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
		if (stationId != m_route.endStationId()) {
			stationName = Route::StationName(stationId).c_str();
		}
		lineId = pos->lineId;
		lineName = Route::LineName(lineId).c_str();

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
	} else {
		GetDlgItem(IDC_BUTTON_BS)->EnableWindow(FALSE);		// Disnable [-] button
	}
	UpdateWindow();
	return idx;
}

//	m_route�̓��e�Ōo�H���X�g:IDC_LIST_ROUTE���쐬���Ȃ���(���3�s�̂�)
//	@return IDC_LIST_ROUTE�̕ύX�s����Ԃ�
//
int Calps_mfcDlg::ModifyRouteList()
{
	int idx;
	int w0;
	int w1;
	int nitem;
	int lineId = 0;
	int stationId = 0;
	DWORD dw;

	w0 = w1 = 0;

	CListCtrl* pLRoute = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE));

	nitem = pLRoute->GetItemCount();
	if (nitem < 5) {
		return UpdateRouteList();	// 5�s�ȉ��Ȃ�S�X�V
	}
	if (m_route.routeList().size() <= 1) {
		ASSERT(FALSE);
		return 0;					// �o�H�Ȃ�
	}
	// List�̌��3�s������
	for (idx = 0; idx < 3; idx++) {
		pLRoute->DeleteItem(nitem - idx - 1);
	}
	idx = nitem - 3 - 1;	// last index
	dw = pLRoute->GetItemData(idx);
	RouteItem ri(IDENT1(dw), IDENT2(dw));	// last item

	vector<RouteItem>::const_reverse_iterator rpos = m_route.routeList().crbegin();
	rpos++;
	// �o�H���X�g���Ō���\����Last Item�l�ƈ�v������̂𓾂�.
	while (rpos != m_route.routeList().crend()) {
		if (ri == *rpos) {
			break;
		}
		rpos++;
	}
	
	ASSERT(ri == *rpos);
	ASSERT(rpos != m_route.routeList().crend());
	ASSERT(rpos != m_route.routeList().crbegin());
	if (!ri.is_equal(*rpos)) {
		return idx;		// safety >>>>>>>>>>>
	}
	
	do {
		rpos--;		// forward
		idx++;		// forward

		CString lineName;
		CString stationName;
		/* 
		 * retrive line and station
		 *
		 */
		stationId = rpos->stationId;
		if (stationId != m_route.endStationId()) {
			stationName = Route::StationName(stationId).c_str();
		}
		lineId = rpos->lineId;
		lineName = Route::LineName(lineId).c_str();

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
	} while (rpos != m_route.routeList().crbegin());

	idx++;
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
void Calps_mfcDlg::showFare()
{
	int opt;
	int flag;
	bool ui_sel;
	
	/*	�^���\�������t���O�擾 */

	flag = 0;

	if (GetDlgItem(IDC_BUTTON_SPECIAL_CITY)->IsWindowEnabled()) {
		CString cap;
		GetDlgItem(IDC_BUTTON_SPECIAL_CITY)->GetWindowText(cap);
		if (0 <= cap.Find(_T("���w"))) {
			flag = APPLIED_START;	// ���w��s��s����(�K�p�́u���{��-�卂�v�ޗ�̂�)
		}
	}
	/* �K���K�p��? */
	flag |=	((BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_RULEAPPLY)) ? RULE_APPLIED : RULE_NO_APPLIED);
	
	SetDlgItemText(IDC_EDIT_RESULT, m_route.showFare(flag).c_str());
	
	opt = m_route.fareCalcOption();
	if ((opt == 1) || (opt == 2)) {
		//case 1:		// �u���w��P�w�Ɏw��v
		//case 2:		// �u���w��P�w�Ɏw��v
		GetDlgItem(IDC_BUTTON_SPECIAL_CITY)->EnableWindow(TRUE);
		if (opt == 1) {
			SetDlgItemText(IDC_BUTTON_SPECIAL_CITY, _T("���w��P�w�Ɏw��"));
		} else {
			SetDlgItemText(IDC_BUTTON_SPECIAL_CITY, _T("���w��P�w�Ɏw��"));
		}
	} else {
		GetDlgItem(IDC_BUTTON_SPECIAL_CITY)->EnableWindow(FALSE);
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

	if (1 < m_route.routeList().size()) {

		vector<RouteItem>::const_iterator pos = m_route.routeList().cbegin();
		TRACE(_T("\nbegin befor: %s\n"), Route::StationName(pos->stationId).c_str());
		for (++pos; pos != m_route.routeList().cend(); pos++) {
			TRACE(_T("%s, %s, %d\n"), Route::LineName(pos->lineId).c_str(), Route::StationName(pos->stationId).c_str(), pos->flag>>31);
		}
		TRACE(_T("\n"));
	
		UpdateRouteList();

		pos = m_route.routeList().cbegin();
		TRACE(_T("\nbegin after: %s\n"), Route::StationName(pos->stationId).c_str());
		for (++pos; pos != m_route.routeList().cend(); pos++) {
			TRACE(_T("%s, %s, %d\n"), Route::LineName(pos->lineId).c_str(), Route::StationName(pos->stationId).c_str(), pos->flag>>31);
		}
		TRACE(_T("\n"));
	} else {
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

	GetDlgItem(IDC_EDIT_START)->SetWindowText(Route::StationName(m_route.startStationId()).c_str());	// ���w�\��

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
	} else {
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

