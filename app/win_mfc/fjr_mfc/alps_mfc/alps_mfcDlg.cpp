
// alps_mfcDlg.cpp : 実装ファイル
//
//	Copyright(c) sutezo9@me.com 2012 allright reserved.
//	This source code is GPL license.

#include "stdafx.h"
#include "alps_mfc.h"
#include "alps_mfcDlg.h"
#include "afxdialogex.h"
#include "TermSel.h"
#include "RouteInputDlg.h"
#include "CQueryNeerest.h"
#include "CInfoMessage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
#include <vector>

// ルート選択[完了]か?
//#define isComplete()	(1 < m_route.routeList().size())



// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
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


// Calps_mfcDlg ダイアログ




Calps_mfcDlg::Calps_mfcDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Calps_mfcDlg::IDD, pParent)
{
	m_selAutorouteOption = 0;
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
	ON_BN_CLICKED(IDC_BUTTON_NEEREST, &Calps_mfcDlg::OnBnClickedButtonNeerest) // deleted
	ON_BN_CLICKED(IDC_MFCMENUBUTTON_FAREOPT, &Calps_mfcDlg::OnBnClickedMfcmenubuttonFareopt)
END_MESSAGE_MAP()


// Calps_mfcDlg メッセージ ハンドラー

BOOL Calps_mfcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
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


	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	//--
	DBsys dbsys;
	RouteUtil::DbVer(&dbsys);
	CString s;

	s.Format(_T(" - DB ver[%s(消費税%d%%):%sGMT]"), dbsys.name, g_tax, dbsys.createdate);
	CString ss;
	GetWindowText(ss);
	SetWindowText(ss + s);

	CListCtrl* pLSel = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_LINESTATIONS));
	pLSel->SetColumnWidth(pLSel->InsertColumn(0, _T("")), LVSCW_AUTOSIZE);
	pLSel->SetColumnWidth(pLSel->InsertColumn(1, _T("")), LVSCW_AUTOSIZE);

	pLSel = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE));
	CRect rc;
	pLSel->GetWindowRect(rc);
	pLSel->InsertColumn(0, _T("路線"), LVCFMT_LEFT, rc.Width() / 5  * 3, 0);
	pLSel->InsertColumn(1, _T("乗換駅"), LVCFMT_LEFT, rc.Width() / 5 * 2, 0);

	ResetContent();	// 発駅、着駅、経路 => 全消去 IDC_BUTTON_ALL_CLEAR [X] Button pushed.

	alert_message(STARTUP);

//////////////////////////////////////#####################
#ifdef _DEBUG
//	test_exec();	// 単体テストを実行(結果はtest_result.txt)
#endif
//////////////////////////////////////#####################

	DragAcceptFiles();

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
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

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void Calps_mfcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR Calps_mfcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//	DialogなのでEnterやESCでウィンドウが閉じないようにするため.
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


// 発駅指定
//	IDC_BUTTON_STARTSEL [...] button pushed
//
void Calps_mfcDlg::OnBnClickedButtonStartsel()
{
	CTermSel dlg(false, this);
	int startStationId;

	if (IDOK == dlg.DoModal()) {
		startStationId = dlg.getStationId();
		if (startStationId != m_route.departureStationId()) {
			// 既選択ルートの破棄確認
			if (0 < reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE))->GetItemCount()) {
				if (IDYES != CancelRouteQuery()) {
					return;
				}
				ResetContent();	// 発駅、着駅、経路 => 全消去 IDC_BUTTON_ALL_CLEAR [X] Button pushed.
			}
			m_curStationId = startStationId;		// 発駅Id
			m_route.add(startStationId);
			GetDlgItem(IDC_EDIT_START)->SetWindowText(dlg.getStationName());	// 発駅表示
			setupForLinelistByStation(m_curStationId);				// 発駅の所属路線一覧の表示
			GetDlgItem(IDC_LIST_LINESTATIONS)->EnableWindow(TRUE);	// 駅/路線 選択リスト選択可
			GetDlgItem(IDC_BUTTON_SEL)->EnableWindow(TRUE);			// [+] button
			GetDlgItem(IDC_BUTTON_AUTOROUTE)->EnableWindow(TRUE);	/* Enable [AutoRoute]ボタン */
		}// 不変更ならなにもしない
	}
}


// 「路線」／「分岐駅」／「着駅」の選択
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
		SetDlgItemText(IDC_EDIT_STAT, _T("直前乗車路線は指定できません.戻って乗換駅を再選択してください."));
		return;
	}

	nRoute	 = pLRoute->GetItemCount();			// 経路リスト数
	selTitle = pLSel->GetItemText(selIdx, 0);	// 選択 駅／線 名
	selId    = pLSel->GetItemData(selIdx);		//        のId

	ASSERT(0 < selId);

	if (m_selMode == SEL_LINE) {
		// add route list 路線Id
        CString str;
        str.Format(_T("%d) %s"), static_cast<int>(nRoute + 1), static_cast<LPCTSTR>(selTitle));
        pLRoute->SetItemText(pLRoute->InsertItem(LVIF_TEXT | LVIF_PARAM, nRoute, str, 0, 0, 0, MAKEPAIR(selId, 0)), 1, _T(""));
		wc = pLRoute->GetColumnWidth(0);
		wn = pLRoute->GetStringWidth(str) + 16;
		if (wc < wn) {
			pLRoute->SetColumnWidth(0, wn);
		}
		GetDlgItem(IDC_BUTTON_BS)->EnableWindow(TRUE);		// Enable [-] button

		m_selMode = SEL_JUNCTION;
		setupForStationlistByLine(selId, m_curStationId, m_route.departureStationId());
	}
	else {
		// add route list 駅Id

		m_route.setNotSameKokuraHakataShinZai(
			(MF_CHECKED & m_menu.GetMenuState(IDR_MENU_KOKURA_HAKATA_BULLET_NOTSAME, MF_BYCOMMAND))
				? true : false);

		/* 経路追加 */// last line
		curLineId = IDENT1(pLRoute->GetItemData(nRoute - 1));	//(stationId is not yet strage in HIWORD)
		TRACE(_T("[route.add(%s-%s)\n"), RouteUtil::LineName(curLineId).c_str(), RouteUtil::StationName(selId).c_str());
		int rslt = m_route.add(curLineId, /*m_curStationId,*/selId);
		if (m_route.isModified()) {
			if (0 < UpdateRouteList()) {		// 経路表示更新
				curLineId = m_route.routeList().back().lineId;
				selId = m_route.routeList().back().stationId;
			}
			else {
				ResetContent();
				return;
			}
		}
		else if ((rslt == 0) || (rslt == 1) || (rslt == 4)) {
			pLRoute->SetItemText(nRoute - 1, 1, selTitle);					// 経路：分岐駅表示
			pLRoute->SetItemData(nRoute - 1, MAKEPAIR(curLineId, selId));	//経路：分岐駅ID設定
																			// カラム幅調整
			wc = pLRoute->GetColumnWidth(1);
			wn = pLRoute->GetStringWidth(selTitle) + 16;
			if (wc < wn) {
				pLRoute->SetColumnWidth(1, wn);
			}
		}
		if (rslt < 0) {
			switch (rslt) {
			case -1:
				SetDlgItemText(IDC_EDIT_STAT, _T("経路が重複しています."));
				break;
			case -4:
				SetDlgItemText(IDC_EDIT_STAT, _T("許可されない会社線通過連絡運輸です."));
				break;
			default:
				ASSERT(FALSE);	/* -1 以外はバグ(DBエラーか、引数不正) */
				AfxMessageBox(_T("Fatal error occured."));
				ResetContent();
				return;
			}
			return;
		}

		if (5 == rslt) {
			SetDlgItemText(IDC_EDIT_STAT, _T("経路は片道条件に達しています."));
			return;	/* already finished */
		}
		if ((0 != rslt) && (1 != rslt) && (4 != rslt)) {	/* fin */
			ASSERT(FALSE);
			return;
		}

		/* result == 1 or 0 or 4 */

		setupForLinelistByStation(selId, curLineId);

		/* 運賃表示 */
		showFare();
		m_curStationId = selId;
		m_selMode = SEL_LINE;	/* for [-] button */
		ASSERT(m_curStationId == (m_route.routeList().cend() - 1)->stationId);
		GetDlgItem(IDC_EDIT_END)->SetWindowText(RouteUtil::StationName((m_route.routeList().cend() - 1)->stationId).c_str());	// 着駅表示

		if (0 == rslt) {
			SetDlgItemText(IDC_EDIT_STAT, _T("経路が片道条件に達しました."));
			MessageBox(_T("経路が片道条件に達しました."), _T("経路終端"),
				MB_OK | MB_ICONINFORMATION);
		}
		else if (4 == rslt) {
			SetDlgItemText(IDC_EDIT_STAT, _T("会社線を含む経路はこれ以上指定できません."));
		}
	}
}

//	List double click
//	IDC_LIST_LINESTATIONS [(路線／分岐駅／着駅)] ListCtrl Double clicked.
//
void Calps_mfcDlg::OnNMDblclkListStations(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnBnClickedButtonSel();	// 「路線」/「分岐駅」/「着駅」の選択(IDC_BUTTON_SEL[+]button pushed)
}

//	分岐駅選択 Radio Button
//	IDC_RADIO_BRANCH_SEL [分岐駅] Radio Button pushed.
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
			(m_curStationId == m_route.departureStationId())) ||
			((itemcnt < 2) || 
		(m_curStationId == IDENT2(pLRoute->GetItemData(pLRoute->GetItemCount() - 2)))));	// current select line

	if (0 < curLineId) {
		setupForStationlistByLine(curLineId, m_curStationId, m_route.departureStationId());
	} else {
		ASSERT(FALSE);
	}
}

//	終着駅指定 Radio button
//	IDC_RADIO_STATION_SEL [着駅] Radio Button pushed.
//
void Calps_mfcDlg::OnBnClickedRadioTerminalSel()
{
	OnBnClickedRadioBranchSel();	// IDC_RADIO_BRANCH_SEL [分岐駅] Radio Button pushed.
}

// delete button(経路の末尾をキャンセル)
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
		/* 路線リスト表示中 */
		ASSERT(2 <= m_route.routeList().size());
		ASSERT(m_curStationId == IDENT2(pLRoute->GetItemData(numList - 1)));

		if (2 <= numList) {			/* Routeリストに分岐駅は2行以上? */
			m_curStationId = IDENT2(pLRoute->GetItemData(numList - 2)); // 最終行の分岐駅
		} else {
			m_curStationId = m_route.departureStationId();		// 発駅
		}
		m_selMode = SEL_JUNCTION;
		m_route.removeTail();

		curLineId = IDENT1(pLRoute->GetItemData(numList - 1));	// last line
		setupForStationlistByLine(curLineId, m_curStationId, m_route.departureStationId());

		pLRoute->SetItemText(numList - 1, 1, _T(""));
		pLRoute->SetItemData(numList - 1, MAKEPAIR(curLineId, 0));

		/* 運賃表示 */
		showFare();
		if (m_route.routeList().size() <= 1) {
			GetDlgItem(IDC_EDIT_END)->SetWindowText(_T(""));	// 着駅表示
		}
		else {
			GetDlgItem(IDC_EDIT_END)->SetWindowText(RouteUtil::StationName((m_route.routeList().cend() - 1)->stationId).c_str());	// 着駅表示
		}
	} else {	/* SEL_JUNCTION or SEL_TERMINATE */
		/* 分岐駅／着駅リスト表示中 */
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

//	発駅、着駅、経路 => 全消去
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


//	IDC_BUTTON_AUTOROUTE [最短経路] button pushed
//	着駅を指定して最短経路を算出
//
void Calps_mfcDlg::OnBnClickedButtonAutoroute()
{
	int rc;
	int endStationId;

	if (m_route.departureStationId() <= 0) {
		MessageBox(_T("開始駅、終了駅を指定しないと最短経路は算出しません."),
										_T("確認"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	CTermSel dlg(true, this);

	if (IDOK != dlg.DoModal()) {
		return;
	}
	endStationId = dlg.getStationId();

	ASSERT(0 < endStationId);

	// 開始駅=終着駅は盲腸線でないことを確認
	if (endStationId == m_route.departureStationId()) {
		if (RouteUtil::NumOfNeerNode(endStationId) <= 1) {
			CantSameStartAndEnd();	// "指定した発駅＝着駅の経路は指定不可能です"
			return;
		}
	}

	if (m_route.routeList().back().stationId == endStationId) {
		MessageBox(_T("開始駅=終了駅では最短経路は算出しません."),
										_T("確認"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	RouteList route(m_route);		// backup to route
	Route test_route(route);
	(void)test_route.changeNeerest(3, endStationId);
	int pass_route = test_route.typeOfPassedLine(m_route.routeList().size());
	// 0 alloff(在来線のみなので選択肢なし)
	// 1 会社線OFF(新幹線はあるが会社線はない)
	// 2 新幹線OFF(会社線はあるが新幹線はない)
	// 3 そのまま(新幹線も会社線もある)(OFFにしない)
	CQueryNeerest queryDlg(pass_route, m_selAutorouteOption, this);
	queryDlg.target.Format(_T("%sまでの最短経路"), RouteUtil::StationNameEx(endStationId).c_str());
	if ((pass_route == 0) || (IDOK == queryDlg.DoModal())) {
		m_selAutorouteOption = queryDlg.choice();
		rc = m_route.changeNeerest((pass_route == 0) ? 0 : m_selAutorouteOption, endStationId);
		if ((rc == 5) || (rc == 0)) {
			if (m_route.isModified()) {
				UpdateRouteList();
			}
			SetDlgItemText(IDC_EDIT_STAT, _T("経路は片道条件に達しています."));
			return;	/* already finished */
		}
		else if (rc == 4) { /* already routed */
			SetDlgItemText(IDC_EDIT_STAT, _T("開始駅へ戻るにはもう少し経路を指定してからにしてください"));
			m_route.assign(route);	/* such as 代々木 新大久保 -> 代々木 */
			return;
		}
		if (0 < rc) {
			int numList = UpdateRouteList();	/* IDC_LIST_ROUTE update view */
			if (0 < numList) {
				ASSERT(m_curStationId == endStationId);
				return;				/* success */
			}
			else {
				ASSERT(FALSE);
				m_route.assign(route);
				UpdateRouteList();	/* IDC_LIST_ROUTE update view */
			}
		}
		else if (-100 < rc) {
			MessageBox(_T("経路が重複しているため算出できませんでした."),
				_T("自動ルート"), MB_OK | MB_ICONEXCLAMATION);
			m_route.assign(route);
			UpdateRouteList();	/* IDC_LIST_ROUTE update view */
		}
		else { /* < -1000 */
			MessageBox(_T("算出できませんでした."),
				_T("確認"), MB_OK | MB_ICONEXCLAMATION);
			m_route.assign(route);
			UpdateRouteList();	/* IDC_LIST_ROUTE update view */
		}
	}
}

//	ListView<IDC_LIST_LINESTATIONS>
//	駅の所属路線一覧を表示
//
//	@param [in]				stationId		駅
//	@param [in][optional]	curLineId	選択不可路線(直前乗車路線)
//
void Calps_mfcDlg::setupForLinelistByStation(int stationId, int curLineId /* =0 */)
{
	CListCtrl* pLSel = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_LINESTATIONS));
	LVCOLUMN column;
	int idx;

	pLSel->DeleteAllItems();			// all record clear

	// 分岐駅<->終着駅 選択
	GetDlgItem(IDC_RADIO_BRANCH_SEL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO_TERMINAL_SEL)->ShowWindow(SW_HIDE);

	memset(&column, 0, sizeof(column));

	CRect rc;
	pLSel->GetClientRect(rc);

	column.mask = LVCF_TEXT | LVCF_WIDTH;
	column.pszText = _T("路線");		// List title
	column.cx = rc.Width();
	pLSel->SetColumn(0, &column);
	column.pszText = _T("");
	column.cx = 10;
	pLSel->SetColumn(1, &column);

	// 駅の所属路線をリスト
	DBO dbo = RouteUtil::Enum_line_of_stationId(stationId);
	if (!dbo.isvalid()) {
		ASSERT(FALSE);
		return;
	}
	for (idx = 0; dbo.moveNext(); idx++) {
		CString lineName;
		int lineId = dbo.getInt(1);

		/* 前回選択路線は選択不可(行頭に"-") */
		lineName.Format(_T("%s%s"), (lineId == curLineId) ? _T("- ") : _T(""), dbo.getText(0).c_str());

		pLSel->SetItemText(
				pLSel->InsertItem(LVIF_TEXT | LVIF_PARAM,
								idx, lineName, 0, 0, 0, lineId),
				1, _T(""));
	}
	ASSERT(1 <= idx);
	SetDlgItemText(IDC_EDIT_STAT, _T("路線を選択してください."));
	SetDlgItemText(IDC_STATIC_LIST_SEL, _T("路線選択"));
}

//	ListView<IDC_LIST_LINESTATIONS>
//	路線内の分岐駅／全駅一覧を表示
//
//	@param [in]	lineId			路線
//	@param [in]	curStationId	選択不可駅
//	@param [in]	startStationId	開始駅(選択可)
//
void Calps_mfcDlg::setupForStationlistByLine(int lineId, int curStationId, int startStationId)
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
		column.pszText = _T("分岐駅");
		SetDlgItemText(IDC_EDIT_STAT, _T("乗換駅を選択してください."));
		SetDlgItemText(IDC_STATIC_LIST_SEL, _T("乗換駅選択"));
	} else {
		CheckRadioButton(IDC_RADIO_BRANCH_SEL, IDC_RADIO_TERMINAL_SEL, IDC_RADIO_TERMINAL_SEL);
		column.pszText = _T("駅");
		SetDlgItemText(IDC_EDIT_STAT, _T("着駅を選択してください."));
		SetDlgItemText(IDC_STATIC_LIST_SEL, _T("着駅選択"));
	}
	pLSel->SetColumn(0, &column);

	column.pszText = _T("分岐路線");
	pLSel->SetColumn(1, &column);

	DBO dbo;
	if (m_selMode == SEL_JUNCTION) {
		dbo = RouteUtil::Enum_junction_of_lineId(lineId, startStationId);
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

	int width1, width2;		// カラム幅計算用変数
	width1 = width2 = 0;

	for (idx = 0; dbo.moveNext(); idx++) {
		CString stationName;
		int stationId = dbo.getInt(1);

		stationName.Format(_T("%s%s"),
			(stationId == curStationId) ? _T("- ") : (stationId == startStationId) ? _T("> ") : _T(""), dbo.getText(0).c_str());

		CString jctLines;
		if (0 != dbo.getInt(2)) { /* 分岐駅 */
			// 分岐駅の乗り入れ路線を'/'で区切り列挙した文字列を作成
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

		// カラム幅計算
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

//	ルート選択キャンセル(ルート破棄）問い合わせ
//
UINT Calps_mfcDlg::CancelRouteQuery()
{
	return MessageBox(_T("経路選択中ですがすべてキャンセルしてもよろしいですか？"),
										_T("選択経路破棄確認"), MB_YESNO | MB_ICONQUESTION);
}

// "指定した発駅＝着駅の経路は指定不可能です"
//
void Calps_mfcDlg::CantSameStartAndEnd()
{
	MessageBox(_T("指定した発駅＝着駅の経路は指定不可能です"), _T("指定不可発着駅"), MB_ICONEXCLAMATION);
}


//	表示クリア
//	(画面表示の初期化)
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

	column.pszText = _T("路線");
	column.cx = 80;
	pLSel->SetColumn(0, &column);

	column.pszText = _T("");
	column.cx = 200;
	pLSel->SetColumn(1, &column);

	pLSel->DeleteAllItems();			// all record clear

	reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE))->DeleteAllItems();

	GetDlgItem(IDC_LIST_LINESTATIONS)->EnableWindow(FALSE);	// 駅/路線 選択リスト
	GetDlgItem(IDC_BUTTON_SEL)->EnableWindow(FALSE);		// [+]button

	GetDlgItem(IDC_BUTTON_BS)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_STAT)->SetWindowText(_T("発駅選択してください."));

	GetDlgItem(IDC_EDIT_START)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_END)->SetWindowText(_T(""));
	GetDlgItem(IDC_STATIC_LIST_SEL)->SetWindowText(_T("")); // ListView title
	GetDlgItem(IDC_RADIO_BRANCH_SEL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_RADIO_TERMINAL_SEL)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_BUTTON_STARTSEL)->SetFocus();

	SetDlgItemText(IDC_EDIT_RESULT, _T(""));

	GetDlgItem(IDC_BUTTON_REVERSE)->EnableWindow(FALSE);/* Disable [Reverse]ボタン */
	GetDlgItem(IDC_BUTTON_AUTOROUTE)->EnableWindow(FALSE);/* Disable [AutoRoute]ボタン */
	GetDlgItem(IDC_BUTTON_NEEREST)->EnableWindow(FALSE);/* Disable [Neerest]ボタン */

	//m_fareOptionMenuButton.DrawMenuBar();

	resetMenu(true);
}


//	メニューリセット
//
void Calps_mfcDlg::resetMenu(bool en_route)
{
	/* [特例適用] */
	m_menu.CheckMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_CHECKED);
	m_menu.EnableMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_BYCOMMAND | MF_GRAYED);

	/* [株主優待券使用] */
	//m_menu.CheckMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_UNCHECKED);
	//m_menu.EnableMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_BYCOMMAND | MF_GRAYED);

	/* [名阪Option] */
	m_menu.ModifyMenu(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_STRING, IDR_MENU_TERM_AGGR, _T("着駅を単駅に指定"));
	m_menu.EnableMenuItem(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_GRAYED);

	/* [大阪環状線遠回り] */
	m_menu.ModifyMenu(IDR_MENU_ARROUND_OSAKAKAN, MF_BYCOMMAND | MF_STRING, IDR_MENU_ARROUND_OSAKAKAN, _T("大阪環状線遠回り"));
	m_menu.EnableMenuItem(IDR_MENU_ARROUND_OSAKAKAN, MF_BYCOMMAND | MF_GRAYED);

	/* [博多小倉新幹線を別線扱い] */
	m_menu.CheckMenuItem(IDR_MENU_KOKURA_HAKATA_BULLET_NOTSAME, MF_UNCHECKED);
	// always enable
	m_menu.EnableMenuItem(IDR_MENU_KOKURA_HAKATA_BULLET_NOTSAME, MF_BYCOMMAND | MF_ENABLED);

	/* [旅客営業取扱基準規程115条(特定都区市内発着)] */
	m_menu.ModifyMenu(IDR_MENU_RULE115, MF_BYCOMMAND | MF_STRING, IDR_MENU_RULE115, _T("旅客営業取扱基準規程115条(特定都区市内発着)"));
	m_menu.EnableMenuItem(IDR_MENU_RULE115, MF_BYCOMMAND | MF_GRAYED);

	/* [指定した経路で運賃計算] */
	m_menu.ModifyMenu(IDR_MENU_NEERORFAR, MF_BYCOMMAND | MF_STRING, IDR_MENU_NEERORFAR, _T("指定した経路で運賃計算"));
	m_menu.EnableMenuItem(IDR_MENU_NEERORFAR, MF_BYCOMMAND | MF_GRAYED);

	if (en_route) {
		//	[博多-小倉 新幹線在来線別線扱い]チェックボタン
		m_menu.CheckMenuItem(IDR_MENU_KOKURA_HAKATA_BULLET_NOTSAME,
			m_route.isNotSameKokuraHakataShinZai() ? MF_CHECKED : MF_UNCHECKED);
		// always enable
			
		// [反転]
		if (!m_route.isAvailableReverse()) {
			/* not enough route */
			GetDlgItem(IDC_BUTTON_REVERSE)->EnableWindow(FALSE);/* Disable [Reverse]ボタン */
			m_menu.CheckMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_UNCHECKED); /* [特例適用]Uncheck状態 */
			GetDlgItem(IDC_BUTTON_NEEREST)->EnableWindow(FALSE);/* Disable [Neerest]ボタン */
		}
		else {
			GetDlgItem(IDC_BUTTON_REVERSE)->EnableWindow(m_route.isRoundTrip()); /* Enable [Reverse]ボタン */
			GetDlgItem(IDC_BUTTON_NEEREST)->EnableWindow(TRUE);/* Disable [Neerest]ボタン */
		}

		// 特例適用
		if (m_route.getRouteFlag().rule_en()) {
			m_menu.EnableMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_BYCOMMAND | MF_ENABLED);
			if (m_route.getRouteFlag().no_rule) {
				// 特例非適用
				m_menu.CheckMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_UNCHECKED);
				m_menu.EnableMenuItem(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_GRAYED);
			}
			else {
				// 特例適用
				m_menu.CheckMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_CHECKED);


				// 大高 大阪 杉本町 着駅・発駅を単駅に指定
				if (!m_route.getRouteFlag().isMeihanCityEnable()) {
					m_menu.EnableMenuItem(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_GRAYED);
				}
				else {	// 「発駅を単駅に指定」
					if (m_route.getRouteFlag().isStartAsCity()) {
						m_menu.ModifyMenu(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_STRING, IDR_MENU_TERM_AGGR,
							_T("発駅を単駅に指定"));
						m_menu.EnableMenuItem(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_ENABLED);
					}
					else {// 「着駅を単駅に指定」
						m_menu.ModifyMenu(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_STRING, IDR_MENU_TERM_AGGR,
							_T("着駅を単駅に指定"));
						m_menu.EnableMenuItem(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_ENABLED);
					}
				}
			}
		}
		else {
			// 特例 無関係
			m_menu.EnableMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_BYCOMMAND | MF_GRAYED);
			m_menu.EnableMenuItem(IDR_MENU_TERM_AGGR, MF_BYCOMMAND | MF_GRAYED);
		}

		// JR東海株主優待券使用
#if 0	// b#20090901
		if (m_route.getRouteFlag().jrtokaistock_enable) {
			m_menu.EnableMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_BYCOMMAND | MF_ENABLED);
			if (m_route.getRouteFlag().jrtokaistock_applied) {
				m_menu.CheckMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_CHECKED);
			}
			else {
				m_menu.CheckMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_UNCHECKED);
			}
		}
		else {
			// JR東海株主優待券使用 選択肢なし(無関係)
			m_menu.CheckMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_UNCHECKED);
			m_menu.EnableMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_BYCOMMAND | MF_GRAYED);
		}
#endif
		// 大阪環状線 内回り／外回り
		if (!m_route.getRouteFlag().is_osakakan_1pass()) {
			m_menu.EnableMenuItem(IDR_MENU_ARROUND_OSAKAKAN, MF_BYCOMMAND | MF_GRAYED);
		}
		else {
			m_menu.EnableMenuItem(IDR_MENU_ARROUND_OSAKAKAN, MF_BYCOMMAND | MF_ENABLED);
			m_menu.ModifyMenu(IDR_MENU_ARROUND_OSAKAKAN, MF_BYCOMMAND | MF_STRING, IDR_MENU_ARROUND_OSAKAKAN,
				m_route.getRouteFlag().osakakan_detour ?
				_T("大阪環状線近回り") : _T("大阪環状線遠回り"));
		}

		// 旅客営業取扱基準規程115条(特定都区市内発着)
		if (!m_route.getRouteFlag().isEnableRule115()) {
			m_menu.EnableMenuItem(IDR_MENU_RULE115, MF_BYCOMMAND | MF_GRAYED);
		}
		else {
			m_menu.EnableMenuItem(IDR_MENU_RULE115, MF_BYCOMMAND | MF_ENABLED);
			m_menu.ModifyMenu(IDR_MENU_RULE115, MF_BYCOMMAND | MF_STRING, IDR_MENU_RULE115,
				m_route.getRouteFlag().isRule115specificTerm() ?
				_T("旅客営業取扱基準規程115条(単駅最安)") :
				_T("旅客営業取扱基準規程115条(特定都区市内発着)"));
		}

		// 指定した経路で運賃計算
		if (!m_route.getRouteFlag().isEnableLongRoute()) {
			m_menu.EnableMenuItem(IDR_MENU_NEERORFAR, MF_BYCOMMAND | MF_GRAYED);
		}
		else {
			m_menu.EnableMenuItem(IDR_MENU_NEERORFAR, MF_BYCOMMAND | MF_ENABLED);
			m_menu.ModifyMenu(IDR_MENU_NEERORFAR, MF_BYCOMMAND | MF_STRING, IDR_MENU_NEERORFAR,
				m_route.getRouteFlag().isLongRoute() ?
				_T("最安経路で運賃計算") : _T("指定した経路で運賃計算"));
		}
	}
}


//	[リバース]Pushボタン
//	経路を逆転する
//
void Calps_mfcDlg::OnBnClickedButtonReverse()
{
	int rc;

	//endStationId = m_route.endStationId();

	if (m_route.routeList().size() <= 1) {
		return;
	}

	rc = m_route.reverse();

	if (0 <= rc) {
		GetDlgItem(IDC_EDIT_START)->SetWindowText(RouteUtil::StationName(m_route.departureStationId()).c_str());	// 発駅表示
		UpdateRouteList();
		if (rc == 0 || rc == 5) {
			SetDlgItemText(IDC_EDIT_STAT, _T("経路は片道条件に達しています."));
		}
	} else if (rc < 0) {
		AfxMessageBox(_T("経路が重複しています"));
	}
}

//	(deleted)
//	[最短経路]
//	すでに発駅、着駅が指定してある経路を最短経路で再構成する
//
void Calps_mfcDlg::OnBnClickedButtonNeerest()
{
	int rc;

	if (m_route.routeList().size() <= 1) {
		return;
	}

	if (m_route.departureStationId() == m_route.arriveStationId()) {
		MessageBox(_T("開始駅=終了駅では最短経路は算出しません."),
			_T("確認"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	Route route;
	RouteList route_backup(m_route);
	rc = route.add(m_route.departureStationId());
	ASSERT(rc == 1);
	rc = route.changeNeerest(0, m_route.arriveStationId());
	if ((rc == 5) || (rc == 0) || (rc == 4)) {
		SetDlgItemText(IDC_EDIT_STAT, _T("経路は片道条件に達しています."));
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
		MessageBox(_T("経路が重複しているため算出できませんでした."),
				   _T("自動ルート"), MB_OK | MB_ICONEXCLAMATION);
		m_route.assign(route_backup);
		UpdateRouteList();	/* IDC_LIST_ROUTE update view */
	} else { /* < -1000  or 0(loop end. never) */
		MessageBox(_T("算出できませんでした."),
				   _T("確認"), MB_OK | MB_ICONEXCLAMATION);
		m_route.assign(route_backup);
		UpdateRouteList();	/* IDC_LIST_ROUTE update view */
	}
}


// [経路記録]
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
		AfxMessageBox(_T("ファイルオープンエラー"));
	}
}

//	[経路開く]
//
void Calps_mfcDlg::OnBnClickedButtonRouteOpen()
{
	CString s;
	CFileDialog dlg(TRUE);
	HINSTANCE hInst =
	ShellExecute(NULL, _T("open"), _T("route.txt"), NULL, NULL, SW_SHOW);

	if (hInst <= (HINSTANCE)32) {
		s.Format(_T("経路記録ファイルのオープンに失敗しました. %d"), hInst);
		AfxMessageBox(s);

		if (IDYES == dlg.DoModal()) {
			ShellExecute(NULL, _T("open"), dlg.GetFileName(), NULL, NULL, SW_SHOW);
		}
	}
}

// [経路入力]
//
void Calps_mfcDlg::OnBnClickedButtonRoutein()
{
	CRouteInputDlg dlg;
	if (IDOK == dlg.DoModal()) {
		parseAndSetupRoute(dlg.routeString);
	}
}

// [結果記録]
//
void Calps_mfcDlg::OnBnClickedButtonResultcopy()
{
	tstring s;
	tstring result_string;
	CStdioFile file;

	CalcRoute croute(m_route);
	FARE_INFO fi;
	croute.calcFare(&fi);
	result_string = fi.showFare(croute.getRouteFlag()).c_str();

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
		AfxMessageBox(_T("ファイルオープンエラー"));
	}
}

//	m_routeの内容でIDC_LIST_ROUTEを作成しなおす
//	@return IDC_LIST_ROUTEの行数を返す
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
	ASSERT((pos->lineId == 0) && (pos->stationId == m_route.departureStationId()));
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
        lineName.Format(_T("%d) %s"), static_cast<int>(idx + 1), static_cast<LPCTSTR>(RouteUtil::LineName(lineId).c_str()));

		pLRoute->SetItemText(
			pLRoute->InsertItem(LVIF_TEXT | LVIF_PARAM,
								idx,
								lineName, 0, 0, 0,
								MAKEPAIR(lineId, stationId)),
								1, stationName);

		// カラム幅計算
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
		// カラム幅設定
		if (pLRoute->GetColumnWidth(0) < w0) {
			pLRoute->SetColumnWidth(0, w0);
		}
		if (pLRoute->GetColumnWidth(1) < w1) {
			pLRoute->SetColumnWidth(1, w1);
		}

		// UI mode set
		m_selMode = SEL_LINE;				/* for [-] button */
		setupForLinelistByStation(stationId, lineId);	// 着駅の所属路線

		m_curStationId = stationId;

		GetDlgItem(IDC_BUTTON_BS)->EnableWindow(TRUE);			// Enable [-] button

		/* 運賃表示 */
		showFare();
		GetDlgItem(IDC_EDIT_END)->SetWindowText(RouteUtil::StationName((m_route.routeList().cend() - 1)->stationId).c_str());	// 着駅表示
	} else {
		GetDlgItem(IDC_BUTTON_BS)->EnableWindow(FALSE);		// Disnable [-] button
	}
	UpdateWindow();
	return idx;
}



/*	Dialog drag and drop
 *  ファイルをD&Dすることで経路を追加する機能
 *	ファイルは単一テキストファイルのみで1行目1行のみ読み込んで設定
 *	1行のみで, ',', ' ', '|', '/', (tab)などの区切りで"駅、路線、分岐駅、路線、..."の並び
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


/*	運賃表示
 *
 */
void Calps_mfcDlg::showFare(bool bResetOption/* = true */)
{
	if (bResetOption) {
		//	[特例適用]チェックボタン
		//	[JR東海株主優待券使用]チェックボタン
		//	[発駅を単駅に指定／着駅を単駅に指定]
		// default settings
		m_route.setNoRule(false);
//b#20090901		m_route.refRouteFlag().setJrTokaiStockApply(false);
	}
	/*	運賃表示条件フラグ取得 */
	CalcRoute croute(m_route);
	FARE_INFO fi;
	croute.calcFare(&fi);
	CString s = fi.showFare(croute.getRouteFlag()).c_str();
	m_route.refRouteFlag().setAnotherRouteFlag(croute.getRouteFlag());

	SetDlgItemText(IDC_EDIT_RESULT, s);

	resetMenu(true);
}


//	Dialog Double click
//
//
void Calps_mfcDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
#ifdef _DEBUG		//	for TEST
	//int numList;

	//CListCtrl* pLRoute = reinterpret_cast<CListCtrl*>(GetDlgItem(IDC_LIST_ROUTE));

	//test_exec();	// 単体テストを実行(結果はtest_result.txt)
	//numList = UpdateRouteList();	/* IDC_LIST_ROUTE update view */

	//m_curStationId = IDENT2(pLRoute->GetItemData(numList - 1));	// last station
	//m_selMode = SEL_LINE;				/* for [-] button */
	//int curLineId = IDENT1(pLRoute->GetItemData(numList - 1));	// last line
	//ASSERT(0 < curLineId);
	//setupForLinelistByStation(m_curStationId, curLineId);	// 着駅の所属路線

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


// 文字列で指定した経路を設定する
//
int Calps_mfcDlg::parseAndSetupRoute(LPCTSTR route_str)
{
	int rc;

	ResetContent();

	rc = m_route.setup_route(route_str);

	GetDlgItem(IDC_EDIT_START)->SetWindowText(RouteUtil::StationName(m_route.departureStationId()).c_str());	// 発駅表示

	switch (rc) {
	case -200:
		AfxMessageBox(_T("駅名不正が含まれています"));
		break;
	case -300:
		AfxMessageBox(_T("線名不正が含まれています"));
		break;
	case -100:
		AfxMessageBox(_T("致命的エラー"));
		break;
	case -1:
		AfxMessageBox(_T("経路が重複しています"));
		break;
	case -2:
		AfxMessageBox(_T("不正な経路指定です"));
		break;
	case -4:
		AfxMessageBox(_T("許可されていない会社線通過です"));
		break;
	case 4:	/* 会社線通過終端 */
	case 0:
	case 1:
		/* success */
		break;
	default:
		AfxMessageBox(_T("不明なエラー"));
		break;
	}
	if (0 <= rc) {
		UpdateRouteList();
		GetDlgItem(IDC_LIST_LINESTATIONS)->EnableWindow(TRUE);	// 駅/路線 選択リスト選択可
		GetDlgItem(IDC_BUTTON_SEL)->EnableWindow(TRUE);			// [+] button
		GetDlgItem(IDC_BUTTON_AUTOROUTE)->EnableWindow(TRUE);	/* Enable [AutoRoute]ボタン */
	}
	else {
		ResetContent();
	}
	return 0;
}

//	[結果を開く]
//
void Calps_mfcDlg::OnBnClickedButtonRsltopen()
{
	CString s;
	CFileDialog dlg(TRUE);
	HINSTANCE hInst =
		ShellExecute(NULL, _T("open"), _T("result.txt"), NULL, NULL, SW_SHOW);

	if (hInst <= (HINSTANCE)32) {
		s.Format(_T("結果記録ファイルのオープンに失敗しました. %d"), hInst);
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

//	[計算オプション...] MenuBarButton
//
void Calps_mfcDlg::OnBnClickedMfcmenubuttonFareopt()
{
	UINT state;
	CString menuTitle;

	switch (m_fareOptionMenuButton.m_nMenuResult) {
	case IDR_MENU_KOKURA_HAKATA_BULLET_NOTSAME:
		//	[博多-小倉 新幹線在来線別線扱い]チェックボタン
		state = m_menu.GetMenuState(IDR_MENU_KOKURA_HAKATA_BULLET_NOTSAME, MF_BYCOMMAND);
		if (MF_CHECKED & state) {
			m_menu.CheckMenuItem(IDR_MENU_KOKURA_HAKATA_BULLET_NOTSAME, MF_UNCHECKED);
			m_route.setNotSameKokuraHakataShinZai(false);
		}
		else {
			m_menu.CheckMenuItem(IDR_MENU_KOKURA_HAKATA_BULLET_NOTSAME, MF_CHECKED);
			m_route.setNotSameKokuraHakataShinZai(true);
			MessageBox(_T("博多-小倉 新幹線在来線別線扱いを選択しました\r\n該当経路は選択しなおす必要があります"), _T("計算オプション"), MB_ICONINFORMATION);
		}
		break;

	case IDR_MENU_ARROUND_OSAKAKAN:
		//	[大阪環状線遠回り／大阪環状線近回り]
		m_menu.GetMenuStringW(IDR_MENU_ARROUND_OSAKAKAN, menuTitle, MF_BYCOMMAND);
		{
			/* 大阪環状線1回だけ通っている? */
			int rc;

			if (m_route.getRouteFlag().is_osakakan_1pass()) {
#if 0
				if (0 <= menuTitle.Find(_T("遠"))) {
					alert_message(OSAKAKAN);
				}
#endif
				ASSERT((m_route.getRouteFlag().osakakan_detour && (0 <= menuTitle.Find(_T("近"))))
					|| (!m_route.getRouteFlag().osakakan_detour && (0 <= menuTitle.Find(_T("遠")))));

				// 近回り時に押されたら遠回り(FAREOPT_OSAKAKAN_DETOUR)に :
				// 遠回り時に押されたら近回り(0:FAREOPT_OSAKAKAN_SHORTCUT)に
				rc = m_route.setDetour(m_route.getRouteFlag().osakakan_detour ? 
					false : true);
				if (1 == rc) {
					showFare(false);
					// optは逆転する
					// または無効となりこのボタンは押せなくなる
				}
				else if (rc != 0) {
					AfxMessageBox(_T("経路が重複しています"));
				}
				else {
					// rc == 0
					int selId = m_route.routeList().back().stationId;

					MessageBox(_T("経路が片道条件に達しました. "), _T("経路終端"), 
						MB_ICONQUESTION);

					CListCtrl* pLRoute = reinterpret_cast<CListCtrl*>(
						GetDlgItem(IDC_LIST_ROUTE));
					int numList = pLRoute->GetItemCount();

					// 2回目は既に空欄となっているので。
					//ASSERT(selId == IDENT2(pLRoute->GetItemData(numList - 1)));	// last station
					ASSERT(m_route.routeList().back().lineId 
						== IDENT1(pLRoute->GetItemData(numList - 1)));	// last line

					pLRoute->SetItemText(numList - 1, 1, _T(""));
					pLRoute->SetItemData(numList - 1, MAKEPAIR(m_route.routeList().back().lineId, 0));

					setupForLinelistByStation(selId, m_route.routeList().back().lineId);
					showFare(false);
				}
			}
			else {
				/* showFare()の後でおこなっているため普通は不要(安全策)*/
				//!		GetDlgItem(IDC_BUTTON_OSAKAKAN)->EnableWindow(FALSE);
			}
		}
		break;

	case IDR_MENU_SPECIFIC_APPLY:
		//	[特例適用]チェックボタン
		state = m_menu.GetMenuState(IDR_MENU_SPECIFIC_APPLY, MF_BYCOMMAND);
		if (MF_CHECKED & state) {
			alert_message(NORULE);
			m_menu.CheckMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_UNCHECKED);
			m_route.setNoRule(true);
		}
		else {
			m_menu.CheckMenuItem(IDR_MENU_SPECIFIC_APPLY, MF_CHECKED);
			m_route.setNoRule(false);
		}
		showFare(false);
		break;

	case IDR_MENU_TERM_AGGR:
		m_menu.GetMenuStringW(IDR_MENU_TERM_AGGR, menuTitle, MF_BYCOMMAND);
		//	[発駅を単駅に指定／着駅を単駅に指定]
		if (0 <= menuTitle.Find(_T("発")))
		{
			ASSERT(!m_route.getRouteFlag().isArriveAsCity());
			m_route.refRouteFlag().setArriveAsCity();
		} else {
			ASSERT(m_route.getRouteFlag().isArriveAsCity());
			m_route.refRouteFlag().setStartAsCity();
		}
		showFare(false);

		break;
#if 0 // b#20090901
	case IDR_MENU_JRTOKAI_STOCK:
		//	[JR東海株主優待券使用]チェックボタン
		state = m_menu.GetMenuState(IDR_MENU_JRTOKAI_STOCK, MF_BYCOMMAND);

		if (MF_CHECKED & state) {
			m_menu.CheckMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_UNCHECKED);
			m_route.refRouteFlag().setJrTokaiStockApply(false);
		}
		else {
			m_menu.CheckMenuItem(IDR_MENU_JRTOKAI_STOCK, MF_CHECKED);
			m_route.refRouteFlag().setJrTokaiStockApply(true);
		}
		showFare(false);
		break;
#endif
	case IDR_MENU_NEERORFAR:
		m_menu.GetMenuStringW(IDR_MENU_NEERORFAR, menuTitle, MF_BYCOMMAND);
		if (0 <= menuTitle.Find(_T("指定した経路"))) {
			m_route.refRouteFlag().setLongRoute(true);
		} else {
			// 単駅最安
			m_route.refRouteFlag().setLongRoute(false);
		}
		showFare(false);
		break;

	case IDR_MENU_RULE115:
		m_menu.GetMenuStringW(IDR_MENU_RULE115, menuTitle, MF_BYCOMMAND);
		// 単駅最安 / 特定都区市内発着
		if (0 <= menuTitle.Find(_T("単駅最安"))) {
			m_route.refRouteFlag().setSpecificTermRule115(false);
		} else {
			// 特定都区市内発着
			m_route.refRouteFlag().setSpecificTermRule115(true);
		}
		showFare(false);
		break;

	default:
		ASSERT(FALSE);
		break;
	}
}



void Calps_mfcDlg::alert_message(int type)
{
	const TCHAR* keys[] =
	{ _T("hide_startup"), _T("hide_osakakan"), _T("hide_norule") };

	const UINT ids[] =
	{ IDS_STARTUP, IDS_ALERTOSAKAKAN, IDS_ALERTNORULE
	};

	if (isKeyExist(keys[type])) {
		return;
	}
	CInfoMessage mesg(this);
	CString msgstr;

	msgstr.LoadString(ids[type]);
	mesg.SetTitleMessage(msgstr);
	if (IDOK == mesg.DoModal()) {
		putKey(keys[type]);
	}
}



