#include "pch.h"
#include "Zip.h"
#include <locale.h>
#include <time.h>

CValkyrieZip::CValkyrieZip()
{
	zf = NULL;
	memset(&m_zfInfo, 0, sizeof(zip_fileinfo));
}

CValkyrieZip::~CValkyrieZip()
{
}

std::tstring MakeDateString(const core::ST_SYSTEMTIME& stTime)
{
	std::tstring strTemp = core::Format(TEXT("%04d-%02d-%02d"), stTime.wYear, stTime.wMonth, stTime.wDay);
	return strTemp;
}

std::tstring MakeUniqueZipFileName(std::tstring strTargetDirectory)
{
	core::ST_SYSTEMTIME stCurTime;
	core::GetSystemTime(&stCurTime);
	
	std::tstring strTargetZipFileName, strTargetZipFile;
	
	strTargetZipFileName = core::Format(TEXT("%s.zip"), MakeDateString(stCurTime).c_str());
	strTargetZipFile = strTargetDirectory + strTargetZipFileName;

	if (!IsFileExist(strTargetZipFile))
		return strTargetZipFile;

	int nIndex = 0;
	do
	{
		nIndex++;
		strTargetZipFileName = core::Format(TEXT("%s(%d).zip"), MakeDateString(stCurTime).c_str(), nIndex);
		strTargetZipFile = strTargetDirectory + strTargetZipFileName;
	} while (IsFileExist(strTargetZipFile));


	return strTargetZipFile;
}

bool CValkyrieZip::Create(std::tstring strTargetDirectory)
{
	bool bRet = false;
	try
	{
		time_t nt = time(NULL);
		tm tmdata;
		errno_t timeError = localtime_s(&tmdata, &nt);

		std::tstring strTargetZipFile = MakeUniqueZipFileName(strTargetDirectory);
		zf = zipOpen(MBSFromTCS(strTargetZipFile).c_str(), APPEND_STATUS_CREATE);
		if(NULL == zf)
			throw exception_format(TEXT("ZipOpen(%s) Failure, %d"), strTargetZipFile.c_str(), ::GetLastError());
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
		return false;
	}
	return true;
}

bool CValkyrieZip::Close()
{
	bool bRet = false;
	try
	{
		zipClose(zf, "");
		if (zf != NULL)
		{
			zf = NULL;
			memset(&m_zfInfo, 0, sizeof(zip_fileinfo));
		}
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
		return false;
	}
	return true;
}

bool CValkyrieZip::Add(std::tstring strFileName, std::vector<BYTE>& vecBinary, std::tstring strPasswd)
{
	if (zf == NULL)
		return false;
	
	try
	{
		time_t nt = time(NULL);
		tm tmdata;
		localtime_s(&tmdata, &nt);

		zip_fileinfo zf_info;
		memset(&zf_info, 0, sizeof(zip_fileinfo));

		zf_info.tmz_date.tm_hour = tmdata.tm_hour;
		zf_info.tmz_date.tm_mday = tmdata.tm_mday;
		zf_info.tmz_date.tm_min = tmdata.tm_min;
		zf_info.tmz_date.tm_mon = tmdata.tm_mon;
		zf_info.tmz_date.tm_sec = tmdata.tm_sec;
		zf_info.tmz_date.tm_year = tmdata.tm_year;
		
		uLong crc = 0;
		if (strPasswd != TEXT(""))
		{
			if (strPasswd != TEXT(""))
			{
				crc = crc32(0L, Z_NULL, 0);
				crc = crc32(crc, (const Bytef*)vecBinary.data(), vecBinary.size());
				if (crc == 0)
				{
					return FALSE;
				}
			}
		}

		int nRet = 0;
		if (strPasswd != TEXT(""))
		{
			nRet = zipOpenNewFileInZip3(zf,
				MBSFromTCS(strFileName).c_str(),
				&zf_info,
				NULL,
				0,
				NULL,
				0,
				"",
				Z_DEFLATED,
				Z_DEFAULT_COMPRESSION,
				0,
				MAX_WBITS,
				DEF_MEM_LEVEL,
				Z_DEFAULT_STRATEGY,
				MBSFromTCS(strPasswd).c_str(),
				crc);
		}
		else
		{
			nRet = zipOpenNewFileInZip(zf,
				MBSFromTCS(strFileName).c_str(),
				&zf_info,
				NULL,
				0,
				NULL,
				0,
				"",
				Z_DEFLATED,
				Z_DEFAULT_COMPRESSION);
		}

		zipWriteInFileInZip(zf, (const void*)vecBinary.data(), vecBinary.size());
		zipCloseFileInZip(zf);
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
		return false;
	}
	return true;
}

