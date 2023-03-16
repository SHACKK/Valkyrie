#pragma once

#define MODE_ACTIVE		0
#define MODE_PASSIVE	1

class CValkyrieFtp
{
	HINTERNET m_hInternetSession;
	HINTERNET m_hFtpSession;
	HINTERNET m_hInternetFind;
	bool m_bDiscovered;
	std::tstring m_strDirectoryCache;

	bool QueryCurrentDir(std::tstring& out);
	bool QueryChangeDirectory(std::tstring strDir);
	
public:
	CValkyrieFtp();
	~CValkyrieFtp();

	bool Open(std::tstring strServerIP, WORD wPort, std::tstring strUserID, std::tstring strUserPass);
	bool Close();
	
	bool IsExistFile(std::tstring strFile);
	bool QueryFileListFrom(core::ST_SYSTEMTIME stDate, std::vector<std::tstring>& out);
	std::tstring QueryFileDir(std::tstring strFileName, std::tstring strStartDirectory = TEXT(""));
	std::tstring QueryFileDirectory(std::tstring strRemoteFileName, std::tstring strStartDirectory = TEXT(""));
	DWORD QueryFile(std::tstring strRemoteFile, std::tstring strLocalDirectory);
	DWORD QueryFileInMemory(std::tstring strFile, std::vector<BYTE>& out);
};

