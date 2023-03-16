#pragma once

class CAboutBoxDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAboutBoxDlg)

public:
	CAboutBoxDlg(CWnd* pParent = nullptr);
	virtual ~CAboutBoxDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
};
