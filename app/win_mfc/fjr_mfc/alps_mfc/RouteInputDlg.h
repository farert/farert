#pragma once


// CRouteInputDlg �_�C�A���O

class CRouteInputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRouteInputDlg)

public:
	CRouteInputDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CRouteInputDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ROUTE_INPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
//	CString routeString;
	CString routeString;
	virtual BOOL OnInitDialog();
};
