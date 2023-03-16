#pragma once

#define REPORT_DIR TEXT("/mnt/2tb/result/report/")
#define SAMPLE_DIR TEXT("/mnt/2tb/sample/malware/")

typedef void* HODIN;
typedef void (*FP_DOWNLOAD_PROGRESS)(LPVOID pContext, int nFileIndex, int nFileCount, std::tstring strHash);
typedef void (*FP_QUERY_PROGRESS)(LPVOID pContext, std::tstring strStatus, E_QUERY_STATE nType);

struct ST_DOWNLOAD_TARGET_FILE
{
	SYSTEMTIME stDate;
	std::tstring strHash;
};

std::tstring IpFromDomain(PCSTR pszDomain, WORD wPort);

HODIN	CreateOdinHandle(ST_FTP_INFO stFtpInfo);
void	DestroyOdinHandle(HODIN hOdin);


ECODE QueryReport(
	HODIN hOdin,
	core::ST_SYSTEMTIME stDate,
	ST_REPORT& out
);

ECODE QueryFileList(
	HODIN hOdin,
	core::ST_SYSTEMTIME stStartDate,
	core::ST_SYSTEMTIME stEndDate,
	std::set<std::tstring> setExt,
	std::vector<ST_TARGET_FILE>& out,
	LPVOID pDlgInst,
	FP_QUERY_PROGRESS fpCallback
);

ECODE DownloadTargetFiles(
	HODIN hOdin,
	std::vector<ST_TARGET_FILE> vecHash,
	ST_ZIP_INFO stOutputFile,
	LPVOID pDlgInst,
	FP_DOWNLOAD_PROGRESS fpCallback
);

ECODE QueryFileDirectory(HODIN hOdin, std::tstring strFileName, std::tstring& strOut, E_QUERY_RANGE nRange = E_RANGE_TOTAL);