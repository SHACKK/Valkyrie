#pragma once

typedef struct ST_FTP_INFO
{
	std::tstring strIP;
	WORD wPort;
	std::tstring strUserID;
	std::tstring strUserPW;
	BOOL bPassive;

	ST_FTP_INFO()
		: strIP()
		, wPort()
		, strUserID()
		, strUserPW()
		, bPassive()
	{};

}ST_FTP_INFO;

typedef struct ST_ZIP_INFO
{
	std::tstring strTargetFile;
	std::tstring strPassword;

	ST_ZIP_INFO()
		: strTargetFile()
		, strPassword()
	{};

}ST_ZIP_INFO;

typedef struct ST_SAMPLE_DATA
{
	std::string strDate;
	std::string strExt;
	std::string strDetection;
	std::string strDetectionName;
	std::string strEngineVersion;
	std::string strElapsedTime;
	std::string strFileName;
	std::string strSHA256;
	std::string strVtPositives;
	std::string	strVtTotal;
	std::string strVtKeyword;

	std::string& operator[] (int nIndex)
	{
		return *(&strDate + nIndex);
	}
}ST_SAMPLE_DATA;

struct ST_REPORT_CONTENTS : public core::IFormatterObject
{
	std::tstring strDate;
	std::tstring strExt;
	std::tstring strDetection;
	std::tstring strDetection_Name;
	std::tstring strEngineName;
	std::tstring strEngine_Version;
	std::tstring strElapsed_Time;
	std::tstring strFileName;
	std::tstring strSHA256;
	std::tstring strVtPositives;
	std::tstring strVtTotal;
	std::tstring strVtKeyword;
	
	void OnSync(core::IFormatter& formatter)
	{
		formatter
			+ core::sPair(TEXT("date"), strDate)
			+ core::sPair(TEXT("ext"), strExt)
			+ core::sPair(TEXT("detection"), strDetection)
			+ core::sPair(TEXT("detection_name"), strDetection_Name)
			+ core::sPair(TEXT("engine_name"), strEngineName)
			+ core::sPair(TEXT("engine_ver"), strDetection_Name)
			+ core::sPair(TEXT("elapsed_time"), strElapsed_Time)
			+ core::sPair(TEXT("file_path"), strFileName)
			+ core::sPair(TEXT("sha256"), strSHA256)
			+ core::sPair(TEXT("vt_positives"), strVtPositives)
			+ core::sPair(TEXT("vt_total"), strVtTotal)
			+ core::sPair(TEXT("vt_keyword"), strVtKeyword)
			;
	}
};

struct ST_REPORT : public core::IFormatterObject
{
	std::vector<ST_REPORT_CONTENTS> vecReportContents;

	void OnSync(core::IFormatter& formatter)
	{
		formatter
			+ core::sPair(TEXT("ReportContents"), vecReportContents)
			;
	}
};

struct ST_TARGET_FILE
{
	std::tstring strDate;
	std::tstring strFileName;
};

enum E_QUERY_STATE
{
	E_QUERY_COMPLETE = 1,
	E_QUERY_FOUND,
	E_QUERY_FAILURE
};

enum E_QUERY_RANGE
{
	E_RANGE_1MONTH,
	E_RANGE_3MONTH,
	E_RANGE_6MONTH,
	E_RANGE_TOTAL
};