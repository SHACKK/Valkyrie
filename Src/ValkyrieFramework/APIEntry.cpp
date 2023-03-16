#include "pch.h"
#include "APIEntry.h"
#include "ValkyrieFramework.h"

std::tstring IpFromDomain(PCSTR pszDomain, WORD wPort)
{
	char szIp[46] = { 0, };
	try
	{
		addrinfo* result = NULL;
		addrinfo* ptr = NULL;
		addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		sockaddr_in* sockaddr_ipv4;
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		std::string strPort = std::to_string(wPort).c_str();
		DWORD dwRes = getaddrinfo(pszDomain, std::to_string(wPort).c_str(), &hints, &result);
		if (dwRes != 0)
			throw core::exception_format(TEXT("Get IP Address Failed, %d"), ::GetLastError());

		for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
		{
			if (ptr->ai_family == AF_INET)
			{
				sockaddr_ipv4 = (sockaddr_in*)ptr->ai_addr;
				inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), (char*)szIp, 46);
			}
		}

		freeaddrinfo(result);

		return TCSFromMBS(szIp);
	}
	catch (std::exception& e)
	{
		core::Log_Error(TEXT("IpFromDomain Failure:%s"), e.what());
		return TEXT("");
	}
}

struct ST_ODIN
{
	CValkyrieFtp Ftp;
	CValkyrieZip Zip;
};

HODIN CreateOdinHandle(ST_FTP_INFO stFtpInfo)
{
	ST_ODIN* stOdin = nullptr;
	try
	{
		stOdin = new ST_ODIN;
		if (stOdin == nullptr)
			throw exception_format(TEXT("new ST_ODIN Failure"));

		if (!stOdin->Ftp.Open(stFtpInfo.strIP, stFtpInfo.wPort, stFtpInfo.strUserID, stFtpInfo.strUserPW))
			throw exception_format(TEXT("Ftp Open Failure"));
	}
	catch (const std::exception& e)
	{
		if (stOdin != nullptr)
			delete stOdin;

		Log_Error("%s", e.what());
		return nullptr;
	}
	return (HODIN)stOdin;
}

void DestroyOdinHandle(HODIN hOdin)
{
	try
	{
		ST_ODIN* stOdinSet = (ST_ODIN*)hOdin;

		if (!stOdinSet->Ftp.Close())
			throw exception_format(TEXT("Ftp Close Failure"));

		delete stOdinSet;
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
	}
}

std::tstring ReportFileNameFrom(ST_SYSTEMTIME stDate)
{
	std::tstring strMonth, strDay;
	if (stDate.wMonth < 10)
		strMonth = Format(TEXT("0%d"), stDate.wMonth);
	else
		strMonth = Format(TEXT("%d"), stDate.wMonth);

	if (stDate.wDay < 10)
		strDay = Format(TEXT("0%d"), stDate.wDay);
	else
		strDay = Format(TEXT("%d"), stDate.wDay);

	return Format(TEXT("%d%s%s.csv"), stDate.wYear, strMonth.c_str(), strDay.c_str());
}

ECODE QueryReport(HODIN hOdin, ST_SYSTEMTIME stDate, ST_REPORT& out)
{
	ECODE nRet = EC_SYSTEM_ERROR;
	try
	{
		ST_ODIN& odin = *(ST_ODIN*)hOdin;
		std::tstring strReportFileName = ReportFileNameFrom(stDate).c_str();
		std::tstring strReportFile = Format(TEXT("/mnt/2tb/result/report/%s"), strReportFileName.c_str());

		if (!odin.Ftp.IsExistFile(strReportFile))
			return EC_NO_DATA;
		
		std::vector<BYTE> vecReportBinary;
		DWORD dwFileSize = odin.Ftp.QueryFileInMemory(strReportFile, vecReportBinary);
		if (dwFileSize == 0)
			throw exception_format(TEXT("QueryFileInMemory(%s) Failure, size 0"), strReportFile.c_str());

		std::tstring strTempReport = Format(TEXT("%s/%s"), GetTempPath().c_str(), strReportFileName.c_str());
		nRet = WriteFileContents(strTempReport, vecReportBinary);
		if (EC_SUCCESS != nRet)
			throw exception_format(TEXT("WriteFileContents(%s) Failure, %d"), strTempReport.c_str(), nRet);

		if (!UTF8::ReadCSVFromFile(&out, strTempReport))
			throw exception_format(TEXT("ReadCSVFromFile(%s) Failure"), strTempReport.c_str());

		core::DeleteFile(strTempReport.c_str());
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
		return nRet;
	}
	return EC_SUCCESS;
}

bool IsSameDate(ST_SYSTEMTIME src, ST_SYSTEMTIME dst)
{
	if (src.wYear != dst.wYear)
		return false;
	if (src.wMonth != dst.wMonth)
		return false;
	if (src.wDay != dst.wDay)
		return false;

	return true;
}

std::tstring MakeDirectoryFrom(std::tstring strDate)
{
	std::vector<std::tstring> vecDate;
	core::TokenizeToArray(strDate, TEXT("-"), vecDate);

	if (vecDate.size() != 3)
		return TEXT("");

	return core::Format(TEXT("%s/%s/%s/"), vecDate[0].c_str(), vecDate[1].c_str(), vecDate[2].c_str());
}

std::tstring MakeDirectoryFrom(ST_SYSTEMTIME stDate)
{
	std::tstring strMonth, strDay;
	if (stDate.wMonth < 10)
		strMonth = Format(TEXT("0%d"), stDate.wMonth);
	else
		strMonth = Format(TEXT("%d"), stDate.wMonth);

	if (stDate.wDay < 10)
		strDay = Format(TEXT("0%d"), stDate.wDay);
	else
		strDay = Format(TEXT("%d"), stDate.wDay);

	return Format(TEXT("%d/%s/%s"), stDate.wYear, strMonth.c_str(), strDay.c_str());
}

void IncreaseDate(ST_SYSTEMTIME& date)
{
	UINT64 UnixTime = UnixTimeFrom(date);
	UnixTime += 86400;
	date = SystemTimeFrom(UnixTime);
}

struct ST_QUERY_FILELIST_PARAM
{
	HODIN hOdin;
	core::ST_SYSTEMTIME stStartDate;
	core::ST_SYSTEMTIME stEndDate;
	std::set<std::tstring> setExt;
	std::vector<ST_TARGET_FILE>* vecOut;
	LPVOID pDlgInst;
	FP_QUERY_PROGRESS fpCallback;
};

int QueryFileListThread(void* pContext)
{
	ST_QUERY_FILELIST_PARAM* pParam = (ST_QUERY_FILELIST_PARAM*)pContext;
	ECODE nRet = EC_SYSTEM_ERROR;
	ST_ODIN& odin = *(ST_ODIN*)pParam->hOdin;

	bool bStopFlag = false;
	ST_SYSTEMTIME stDate = pParam->stStartDate;
	
	while (!bStopFlag)
	{
		std::vector<std::tstring> vecFiles;
		std::tstring strDate = MakeDirectoryFrom(stDate);

		if (!odin.Ftp.QueryFileListFrom(stDate, vecFiles))
		{
			core::Log_Error(TEXT("QueryFileListFrom(%s) Failure"), strDate.c_str());
			std::tstring strMsg = Format(TEXT("%s : %d 개의 파일"), strDate.c_str(), vecFiles.size());
			pParam->fpCallback(pParam->pDlgInst, strMsg, E_QUERY_FAILURE);
		}

		// 확장자 필터링
		size_t nTargetFileCount = pParam->vecOut->size();
		for (std::tstring& strFile : vecFiles)
		{
			if (pParam->setExt.find(MakeLower(ExtractFileExt(strFile).c_str())) == pParam->setExt.end())
				continue;

			ST_TARGET_FILE stTargetFile;
			stTargetFile.strDate = strDate;
			stTargetFile.strFileName = strFile;
			pParam->vecOut->push_back(stTargetFile);
		}

		std::tstring strMsg = core::Format(TEXT("%s : %d 개의 파일"), strDate.c_str(), pParam->vecOut->size() - nTargetFileCount);
		pParam->fpCallback(pParam->pDlgInst, strMsg, E_QUERY_FOUND);

		bStopFlag = IsSameDate(stDate, pParam->stEndDate);
		if (!bStopFlag)
			IncreaseDate(stDate);
	}
	pParam->fpCallback(pParam->pDlgInst, TEXT("완료"), E_QUERY_COMPLETE);
	return 0;
}

ECODE QueryFileList(HODIN hOdin, ST_SYSTEMTIME stStartDate, ST_SYSTEMTIME stEndDate, std::set<std::tstring> setExt, std::vector<ST_TARGET_FILE>& vecOut, LPVOID pDlgInst, FP_QUERY_PROGRESS fpCallback)
{
	ST_QUERY_FILELIST_PARAM* param = new ST_QUERY_FILELIST_PARAM;
	param->hOdin = hOdin;
	param->stStartDate = stStartDate;
	param->stEndDate = stEndDate;
	param->setExt = setExt;
	param->vecOut = &vecOut;
	param->pDlgInst = pDlgInst;
	param->fpCallback = fpCallback;

	HANDLE hHandle = core::CreateThread(QueryFileListThread, param);
	if (hHandle == NULL)
		return EC_SYSTEM_ERROR;

	core::CloseThreadHandle(hHandle);
	return EC_SUCCESS;
}

struct ST_DOWNLOAD_THREAD_PARAM
{
	HODIN hOdin;
	std::vector<ST_TARGET_FILE> vecTargetFiles;
	ST_ZIP_INFO stZipInfo;
	LPVOID pDlgInst;
	FP_DOWNLOAD_PROGRESS fpCallback;
};

int DownloadTargetFilesThread(void* pContext)
{
	ST_DOWNLOAD_THREAD_PARAM* pParam = (ST_DOWNLOAD_THREAD_PARAM*)pContext;
	ST_ODIN& odin = *(ST_ODIN*)pParam->hOdin;

	ECODE nRet = EC_SYSTEM_ERROR;

	ST_SYSTEMTIME stCurrentTime;
	GetSystemTime(&stCurrentTime);

	CValkyrieZip zip;
	if (!zip.Create(pParam->stZipInfo.strTargetFile))
	{
		Log_Error("ZipFile(%s) Failure", pParam->stZipInfo.strTargetFile.c_str());
		return -1;
	}

	std::vector<std::tstring> vecFailedList;
	int nIndex = 0;
	for (ST_TARGET_FILE& target : pParam->vecTargetFiles)
	{
		try
		{
			nIndex++;
			std::tstring strTargetFileDirectory = core::Format(TEXT("%s/%s"), SAMPLE_DIR, target.strDate.c_str());
			std::tstring strTargetFile = core::Format(TEXT("%s/%s"), strTargetFileDirectory.c_str(), target.strFileName.c_str());
			std::vector<BYTE> vecBinary;
			DWORD dwFileSize = odin.Ftp.QueryFileInMemory(strTargetFile, vecBinary);
			if (dwFileSize == 0)
				throw exception_format(TEXT("QueryFileInMemory(%s) Failure, size 0"), target.strFileName.c_str());

			if (!zip.Add(target.strFileName, vecBinary, pParam->stZipInfo.strPassword))
				throw exception_format(TEXT("ZipFile(%s) Failure"), target.strFileName.c_str());

			pParam->fpCallback(pParam->pDlgInst, nIndex, pParam->vecTargetFiles.size(), target.strFileName);
		}
		catch (const std::exception& e)
		{
			nIndex--;
			Log_Error("%s", e.what());
			vecFailedList.push_back(target.strFileName);
		}
	}

	if (nIndex != pParam->vecTargetFiles.size())
	{
		for (std::tstring& FailedFile : vecFailedList)
			pParam->fpCallback(pParam->pDlgInst, -1, -1, FailedFile);
	}

	zip.Close();

	return 0;
}

ECODE DownloadTargetFiles(HODIN hOdin, std::vector<ST_TARGET_FILE> vecTargetFiles, ST_ZIP_INFO stZipInfo, LPVOID pDlgInst, FP_DOWNLOAD_PROGRESS fpCallback)
{
	ST_DOWNLOAD_THREAD_PARAM* pParam = new ST_DOWNLOAD_THREAD_PARAM;
	pParam->hOdin = hOdin;
	pParam->vecTargetFiles = vecTargetFiles;
	pParam->stZipInfo = stZipInfo;
	pParam->pDlgInst = pDlgInst;
	pParam->fpCallback = fpCallback;

	HANDLE hHandle = core::CreateThread(DownloadTargetFilesThread, pParam);
	if (hHandle == NULL)
		return EC_SYSTEM_ERROR;

	core::CloseThreadHandle(hHandle);
	return EC_SUCCESS;
}

ECODE QueryFileDirectory(HODIN hOdin, std::tstring strFileName, std::tstring& strOut, E_QUERY_RANGE nRange)
{
	ECODE nRet = EC_NO_DATA;
	
	ST_ODIN& odin = *(ST_ODIN*)hOdin;
	
	try
	{
		core::ST_SYSTEMTIME stCurrentTime, stLimitTime;
		core::GetSystemTime(&stCurrentTime);

		switch (nRange)
		{
		case E_RANGE_1MONTH:
		{
			UINT64 UnixTime = core::UnixTimeFrom(stCurrentTime);
			UnixTime -= 60 * 60 * 24 * 30;
			stLimitTime = core::SystemTimeFrom(UnixTime);
			break;
		}
		case E_RANGE_3MONTH:
		{
			UINT64 UnixTime = core::UnixTimeFrom(stCurrentTime);
			UnixTime -= 60 * 60 * 24 * 30 * 3;
			stLimitTime = core::SystemTimeFrom(UnixTime);
			break;
		}
		case E_RANGE_6MONTH:
		{
			UINT64 UnixTime = core::UnixTimeFrom(stCurrentTime);
			UnixTime -= 60 * 60 * 24 * 30 * 6;
			stLimitTime = core::SystemTimeFrom(UnixTime);
			break;
		}
		case E_RANGE_TOTAL:
		{
			stLimitTime.wYear = 2021;
			stLimitTime.wMonth = 1;
			stLimitTime.wDay = 1;
			break;
		}
		}

		bool bFound = false;
		std::tstring strSearchTargetFile;
		while (true)
		{
			if (IsSameDate(stCurrentTime, stLimitTime))
				break;
			
			std::tstring strMonth, strDay;
			if (stCurrentTime.wMonth < 10) strMonth = core::Format(TEXT("0%d"), stCurrentTime.wMonth);
			else strMonth = core::Format(TEXT("%d"), stCurrentTime.wMonth);

			if (stCurrentTime.wDay < 10) strDay = core::Format(TEXT("0%d"), stCurrentTime.wDay);
			else strDay = core::Format(TEXT("%d"), stCurrentTime.wDay);

			strSearchTargetFile = core::Format(TEXT("%s/%d/%s/%s/%s"), SAMPLE_DIR, stCurrentTime.wYear, strMonth.c_str(), strDay.c_str(), strFileName.c_str());

			if (!odin.Ftp.IsExistFile(strSearchTargetFile))
			{
				UINT64 UnixTime = core::UnixTimeFrom(stCurrentTime);
				stCurrentTime = core::SystemTimeFrom(UnixTime - 86400);
				continue;
			}
			
			bFound = true;
			break;
		}
		
		if (!bFound)
			return EC_NO_DATA;
		
		strOut = strSearchTargetFile;
		
	}
	catch (const std::exception& e)
	{
		core::Log_Error("%s", e.what());
		return nRet;
	}
	return EC_SUCCESS;
}
