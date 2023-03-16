#include "pch.h"
#include "framework.h"
#include "Valkyrie.h"
#include "DownloadProgressDlg.h"
#include "SetUpDlg.h"
#include "ManualDlg.h"
#include "Updator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "MainDlg.h"


BEGIN_MESSAGE_MAP(CValkyrieApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CValkyrieApp::CValkyrieApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}


CValkyrieApp theApp;

BOOL CValkyrieApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	AfxEnableControlContainer();

	CShellManager *pShellManager = new CShellManager;

	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("로컬 애플리케이션 마법사에서 생성된 애플리케이션"));

#ifndef _DEBUG
	// 최신 업데이트 확인
	CUpdator updator;
	if (EC_MORE_DATA == updator.IsUpToDate() &&
		IDYES == MessageBox(NULL, TEXT("최신 업데이트가 발견되었습니다.\r\n지금 업데이트하시겠습니까?"), TEXT("자동 업데이트"), MB_YESNO | MB_ICONINFORMATION))
	{
		ECODE nRet = updator.UpdateThisProduct();
		if (EC_SUCCESS == nRet)
			return FALSE;

		{
			std::tstring strError = core::Format(TEXT("업데이트에 실패하였습니다.\r\n에러코드:%d"), nRet);
			MessageBox(NULL, strError.c_str(), TEXT("에러"), MB_ICONERROR);
		}
	}
#endif

	
	CFileFind pFind;
	BOOL bRet = pFind.FindFile(IniSet->m_strIniFile.c_str());
	INT_PTR nSetupResult = IDOK;
	if (!bRet)
	{
		TRACE(traceAppMsg, 0, _T("Valkyrie 설정 파일이 존재하지 않습니다!\n 프로그램 초기설정 마법사로 이동합니다."));
		CSetUpDlg dlg;
		nSetupResult = dlg.DoModal();
	}

	if (nSetupResult != IDOK)
		return FALSE;

	IniSet->LoadIniFile();

	CMainDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

	#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
			ControlBarCleanUp();
	#endif
	return FALSE;
}

