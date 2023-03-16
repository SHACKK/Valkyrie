#include "pch.h"
#include "Ftp.h"


CValkyrieFtp::CValkyrieFtp()
{
	m_hInternetSession = NULL;
	m_hFtpSession = NULL;
	m_hInternetFind = NULL;
	m_bDiscovered = FALSE;
	m_strDirectoryCache = TEXT("");
}

CValkyrieFtp::~CValkyrieFtp()
{

}

bool CValkyrieFtp::Open(std::tstring strServerIP, WORD wPort, std::tstring strUserID, std::tstring strUserPass)
{
	try
	{
		std::tstring strCurExeFileName = ExtractFileName(GetFileName());

		m_hInternetSession = InternetOpen(strCurExeFileName.c_str(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (m_hInternetSession == nullptr)
			throw exception_format(TEXT("InternetOpen(%s) Failure, %d", strCurExeFileName.c_str(), ::GetLastError()));

		m_hFtpSession = InternetConnect(m_hInternetSession, strServerIP.c_str(), wPort, strUserID.c_str(), strUserPass.c_str(), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
		if (m_hFtpSession == nullptr)
			throw exception_format(TEXT("InternetConnect, %d \nIP : %s\n Port : %d \nUser : %s, \nPassword : %s"), 
									::GetLastError(), strServerIP.c_str(), wPort, strUserID.c_str(), strUserPass.c_str());
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
		return false;
	}
	return true;
}

bool CValkyrieFtp::Close()
{
	try
	{
		if (m_hFtpSession != nullptr)
		{
			if (!InternetCloseHandle(m_hFtpSession))
				throw exception_format(TEXT("InternetCloseHandle(%p) Failure"), m_hFtpSession);
			m_hFtpSession = nullptr;
		}

		if (m_hInternetSession != nullptr)
		{
			if(!InternetCloseHandle(m_hInternetSession))
				throw exception_format(TEXT("InternetCloseHandle(%p) Failure"), m_hInternetSession);
			m_hInternetSession = nullptr;
		}
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
		return false;
	}
	return true;
}

bool CValkyrieFtp::QueryCurrentDir(std::tstring& out)
{
	try
	{
		if (m_hFtpSession == nullptr)
			throw exception_format(TEXT("FtpSession is not opened"));

		DWORD dwSize = MAX_PATH;
		TCHAR szBuffer[MAX_PATH] = { 0, };
		if (!FtpGetCurrentDirectory(m_hFtpSession, szBuffer, &dwSize))
			throw exception_format(TEXT("FtpGetCurrentDirectory Failure, %d"), ::GetLastError());
		
		out = szBuffer;
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
		return false;
	}
	return true;
}

bool CValkyrieFtp::QueryChangeDirectory(std::tstring strDir)
{
	try
	{
		if (m_hFtpSession == nullptr)
			throw exception_format(TEXT("FtpSession is not opened"));
		
		if (!FtpSetCurrentDirectory(m_hFtpSession, strDir.c_str()))
			throw exception_format(TEXT("FtpSetCurrentDirectory(%s) Failure, %d"), strDir.c_str(), ::GetLastError());
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
		return false;
	}
	return true;
}

bool CValkyrieFtp::IsExistFile(std::tstring strFile)
{
	try
	{
		WIN32_FIND_DATA find;
		m_hInternetFind = FtpFindFirstFile(m_hFtpSession, strFile.c_str(), &find, INTERNET_FLAG_RELOAD, 0);
		if (m_hInternetFind == nullptr)
			throw exception_format(TEXT("FtpFindFirstFile(%s) Failure, %d"), strFile.c_str(), ::GetLastError());

		InternetCloseHandle(m_hInternetFind);
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
		return false;
	}
	
	return true;
}

bool CValkyrieFtp::QueryFileListFrom(core::ST_SYSTEMTIME stDate, std::vector<std::tstring>& out)
{
	try
	{
		if (m_hFtpSession == nullptr)
			throw exception_format(TEXT("FtpSession is not opened"));
		
		std::tstring strYear = StringFrom(stDate.wYear);
		std::tstring strMonth, strDay;
		if (9 < stDate.wMonth)
			strMonth = StringFrom(stDate.wMonth);
		else
			strMonth = Format(TEXT("0%d"), stDate.wMonth);
		
		if( 9 < stDate.wDay)
			strDay = StringFrom(stDate.wDay);
		else
			strDay = Format(TEXT("0%d"), stDate.wDay);
		
		std::tstring strOriginDirectory, strTargetDirectory;
		if (!QueryCurrentDir(strOriginDirectory))
			throw exception_format(TEXT("QueryCurrentDir Failure"));
		
		strTargetDirectory = Format(TEXT("%s/%s/%s/%s"), strOriginDirectory.c_str(), strYear.c_str(), strMonth.c_str(), strDay.c_str());
		
		if (!QueryChangeDirectory(MakeFormalPath(strTargetDirectory)))
			throw exception_format(TEXT("QueryChangeDirectory(%s) Failure"), strTargetDirectory.c_str());
		
		WIN32_FIND_DATA w32FindData;
		BOOL bContinue = TRUE;
		memset(&w32FindData, 0, sizeof(WIN32_FIND_DATA));
		m_hInternetFind = FtpFindFirstFile(m_hFtpSession, TEXT("*"), &w32FindData, INTERNET_FLAG_RELOAD, 0);
		while (bContinue)
		{
			bContinue = InternetFindNextFile(m_hInternetFind, &w32FindData);
			out.push_back(w32FindData.cFileName);
		}
		InternetCloseHandle(m_hInternetFind);
		
		if (!QueryChangeDirectory(strOriginDirectory))
			throw exception_format(TEXT("QueryChangeDirectory(%s) Failure"), strOriginDirectory.c_str());
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
		return false;
	}
	return true;
}

std::tstring CValkyrieFtp::QueryFileDir(std::tstring strFileName, std::tstring strStartDirectory)
{
	std::tstring strRet = TEXT("");
	try
	{
		if (m_hFtpSession == nullptr)
			throw exception_format(TEXT("FtpSession is not opened"));
		
		m_bDiscovered = false;
		m_hInternetFind = nullptr;
		WIN32_FIND_DATA w32FindData;
		BOOL bContinue = TRUE;
		memset(&w32FindData, 0, sizeof(WIN32_FIND_DATA));
		
		if (!m_strDirectoryCache.empty())
		{
			std::tstring strFile = Format(TEXT("%s/%s"), m_strDirectoryCache.c_str(), strFileName.c_str());
			m_hInternetFind = FtpFindFirstFile(m_hFtpSession, strFile.c_str(), &w32FindData, INTERNET_FLAG_RELOAD, 0);
			if (m_hInternetFind != NULL)
			{
				if (!SafeStrCmp(w32FindData.cFileName, strFileName.c_str(), MAX_PATH))
				{
					m_bDiscovered = TRUE;
					InternetCloseHandle(m_hInternetFind);

					return m_strDirectoryCache;
				}
				InternetCloseHandle(m_hInternetFind);
			}
		}
		
		std::vector<std::tstring> vecChildDirectory;
		std::tstring strCurrentDirectory;
		if (!QueryCurrentDir(strCurrentDirectory))
			throw exception_format(TEXT("QueryCurrentDir Failure"));
		
		m_hInternetFind = FtpFindFirstFile(m_hFtpSession, NULL, &w32FindData, INTERNET_FLAG_RELOAD, 0);
		if(m_hInternetFind == nullptr)
			throw exception_format(TEXT("FtpFindFirstFile Failure, %d"), ::GetLastError());
		
		do
		{
			bContinue = InternetFindNextFile(m_hInternetFind, &w32FindData);
			if (!SafeStrCmp(w32FindData.cFileName, strFileName.c_str(), MAX_PATH))
			{
				m_bDiscovered = TRUE;
				m_strDirectoryCache = strCurrentDirectory;
				InternetCloseHandle(m_hInternetFind);
				m_hInternetFind = nullptr;
				
				return strCurrentDirectory;
			}
			
			if (w32FindData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
				vecChildDirectory.push_back(Format(TEXT("%s/%s"), strCurrentDirectory.c_str(), w32FindData.cFileName));
		} while (bContinue);

		std::sort(vecChildDirectory.begin(), vecChildDirectory.end());
		std::vector<std::tstring>::reverse_iterator iter;
		for (iter = vecChildDirectory.rbegin(); iter != vecChildDirectory.rend(); iter++)
		{
			if (m_bDiscovered)
			{
				if (m_hInternetFind != NULL)
				{
					InternetCloseHandle(m_hInternetFind);
					m_hInternetFind = NULL;
				}

				return m_strDirectoryCache;
			}

			QueryChangeDirectory(*iter);
			InternetCloseHandle(m_hInternetFind);
			// 반환값 유실됨
			this->QueryFileDir(strFileName, *iter);
		}

		if (m_hInternetFind != NULL)
		{
			InternetCloseHandle(m_hInternetFind);
			m_hInternetFind = NULL;
		}
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
		return strRet;
	}
	
	return strRet;
}

DWORD CValkyrieFtp::QueryFile(std::tstring strFile, std::tstring strLocalDirectory)
{
	DWORD dwDownloadFileSize = 0;
	try
	{
		std::tstring strFileDir = this->QueryFileDir(strFile);
		if (strFileDir.empty())
			throw exception_format(TEXT("QueryFileDir(%s) Failure"), strFile.c_str());
		
		std::tstring strRemoteFile = Format(TEXT("%s/%s"), strFileDir.c_str(), strFile.c_str());

		std::vector<BYTE> vecBuffer;
		dwDownloadFileSize = this->QueryFileInMemory(strRemoteFile, vecBuffer);
		if (0 == dwDownloadFileSize)
			throw exception_format(TEXT("QueryFileInMemory(%s) Failure"), strRemoteFile.c_str());

		std::tstring strLocalFile = Format(TEXT("%s/%s"), strLocalDirectory.c_str(), strFile.c_str());
		
		ECODE nRes = WriteFileContents(strLocalFile, vecBuffer);
		if(nRes != EC_SUCCESS)
			throw exception_format(TEXT("WriteFileContents(%s) Failure"), strLocalFile.c_str());
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
		return 0;
	}
	return dwDownloadFileSize;
}

DWORD CValkyrieFtp::QueryFileInMemory(std::tstring strFile, std::vector<BYTE>& out)
{
	DWORD dwRead = 0;
	try
	{
		if(m_hFtpSession == nullptr)
			throw exception_format(TEXT("FtpSession is not opened"));
		
		HINTERNET hFile = FtpOpenFile(m_hFtpSession, strFile.c_str(), GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0);
		if (hFile == nullptr)
			throw exception_format(TEXT("FtpOpenFile(%s) Failure, %d"), strFile.c_str(), ::GetLastError());
		
		DWORD dwFileSize = FtpGetFileSize(hFile, NULL);
		
		if (dwFileSize == 0)
			throw exception_format(TEXT("FtpGetFileSize(%s) Failure, %d"), strFile.c_str(), ::GetLastError());

		out.resize(dwFileSize);
		
		if (!InternetReadFile(hFile, out.data(), dwFileSize, &dwRead))
			throw exception_format(TEXT("InternetReadFile(%s) Failure, %d"), strFile.c_str(), ::GetLastError());
		
		if(dwFileSize != dwRead)
			throw exception_format(TEXT("InternetReadFile Failurem %d \nFileSize = %d \nReadSize = %d"), ::GetLastError(), dwFileSize, dwRead);

		InternetCloseHandle(hFile);
	}
	catch (const std::exception& e)
	{
		out.resize(0);
		Log_Error("%s", e.what());
		return 0;
	}
	return dwRead;
}
