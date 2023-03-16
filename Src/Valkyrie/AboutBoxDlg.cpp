#include "pch.h"
#include "Valkyrie.h"
#include "AboutBoxDlg.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CAboutBoxDlg, CDialogEx)

CAboutBoxDlg::CAboutBoxDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ABOUTDLG, pParent)
{

}

CAboutBoxDlg::~CAboutBoxDlg()
{
}

void CAboutBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutBoxDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CAboutBoxDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


void CAboutBoxDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}
