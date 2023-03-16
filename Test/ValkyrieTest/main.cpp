#include "../ValkyrieFramework/ValkyrieFramework.h"

#pragma comment(lib, "cppcore.lib")
#pragma comment(lib, "zlibstatic.lib")
#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "Ws2_32.lib")

using namespace core;

void ZipTest()
{
	try
	{
		CValkyrieZip zip;
		if (!zip.Create(TEXT("D:\\GIT\\valkyrie\\Src\\ZipTest.zip")))
			throw exception_format(TEXT("Zip Create(%p) Failure"), zip);
		
		std::vector<std::tstring> vecFiles;
		std::tstring strTargetDirectory = TEXT("D:\\GIT\\valkyrie\\Src\\ValkyrieFramework\\");
		if(EC_SUCCESS != GrepFilesRecursively(strTargetDirectory.c_str(), TEXT("*"), vecFiles))
			throw exception_format(TEXT("GrepFilesRecursively Failure"));

		std::vector<BYTE> vecFileContents;
		for (std::tstring& strFile : vecFiles)
		{
			if(EC_SUCCESS != ReadFileContents(strFile, vecFileContents))
				throw exception_format(TEXT("ReadFileContents(%s) Failure"), strFile.c_str());
			zip.Add(strFile, vecFileContents);
		}

		if(!zip.Close())
			throw exception_format(TEXT("Zip Close(%p) Failure"), zip);
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
	}
}

void FtpTest()
{
	try
	{
		WSADATA wsa;
		if (0 != WSAStartup(MAKEWORD(2, 2), &wsa))
			throw exception_format(TEXT("WSAStartup Failure"));
		
		CValkyrieFtp ftp;
		std::tstring strIP = IpFromDomain("akdan.iptime.org", 21);
		if (!ftp.Open(strIP.c_str(), 21, TEXT("valkyrie"), TEXT("qwe123!@#")))
			throw exception_format(TEXT("Ftp Open(%p) Failure"), ftp);

		ST_SYSTEMTIME stDate;
		GetSystemTime(&stDate);
		stDate.wDay -= 1;
		std::vector<std::tstring> vecFiles;
		if(!ftp.QueryFileListFrom(stDate, vecFiles))
			throw exception_format(TEXT("Ftp QueryFileListFrom(%s) Failure"), StringFrom(stDate).c_str());

		std::tstring strTargetFile = TEXT("012f2a099189f63fdcdb669d62c71fe0145703894a74760fc1ab1ee5df9cb15f.exe");
		std::tstring strFile = ftp.QueryFileDir(strTargetFile);
		if (strFile.empty())
			throw exception_format(TEXT("Ftp QueryFileDir(%s) Failure"), strTargetFile.c_str());

		std::vector<BYTE> vecFileBinary;
		if (0 == ftp.QueryFileInMemory(strTargetFile, vecFileBinary))
			throw exception_format(TEXT("QueryFileInMemory(%s) failure"), strTargetFile.c_str());

		if (vecFileBinary.size() != ftp.QueryFile(strTargetFile, TEXT("D:\\Samples\\")))
			throw exception_format(TEXT("QueryFile(%s) Failure"), strTargetFile.c_str());

		ftp.Close();

		WSACleanup();
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
	}
}

void ApiTest()
{
	try
	{
		ST_FTP_INFO ftp;
		ftp.bPassive = false;
		ftp.strIP = TEXT("222.99.202.173");
		ftp.wPort = 21;
		ftp.strUserID = TEXT("valkyrie");
		ftp.strUserPW = TEXT("qwe123!@#");

		HODIN hOdin = nullptr;
		hOdin = CreateOdinHandle(ftp);
		if (!hOdin)
			throw exception_format(TEXT("CreateOdinHandle Failure"));

		ST_SYSTEMTIME stCurDate;
		GetSystemTime(&stCurDate);
		stCurDate.wDay -= 11;

		ST_REPORT report;
		if(EC_SUCCESS != QueryReport(hOdin, stCurDate, report))
			throw exception_format(TEXT("QueryReport (%s) Failure"), StringFrom(stCurDate).c_str());

		ST_SYSTEMTIME stDstDate = stCurDate;
		stDstDate.wDay += 10;

		std::set<std::tstring> setTargetExt;
		setTargetExt.insert(TEXT("vbs"));
		setTargetExt.insert(TEXT("vba"));

		std::set<std::tstring> setTargetDetection;
		setTargetDetection.insert(TEXT("Benign"));
		setTargetDetection.insert(TEXT("Suspicious"));
		setTargetDetection.insert(TEXT("Malicious"));

		std::vector<ST_TARGET_FILE> strTargetFiles;
		if(EC_SUCCESS != QueryFileList(hOdin, stCurDate, stDstDate, setTargetExt, setTargetDetection, strTargetFiles, nullptr, nullptr))
			throw exception_format(TEXT("QueryFileList (%s ~ %s) Failure"), StringFrom(stCurDate).c_str(), StringFrom(stDstDate).c_str());

		DestroyOdinHandle(hOdin);
	}
	catch (const std::exception& e)
	{
		Log_Error("%s", e.what());
	}
}

int main()
{
	//ZipTest();
	//FtpTest();
	ApiTest();
	
	return 0;
}