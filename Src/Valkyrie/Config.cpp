#include "pch.h"
#include "Config.h"

CConfig::CConfig()
{
    TCHAR* pszConfigDirectory;
    ::SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &pszConfigDirectory);
    std::tstring strConfigDirectory = std::tstring(pszConfigDirectory) + TEXT("/Valkyrie");
    if (!core::IsFileExist(strConfigDirectory))
        core::CreateDirectory(strConfigDirectory.c_str());
	
    m_strJsonFile = core::Format(TEXT("%s/%s"), strConfigDirectory.c_str(), TEXT("ExtConfig.json"));
    if (core::IsFileExist(m_strJsonFile))
        core::ReadJsonFromFile(this, m_strJsonFile);
    else
	{
        m_setTargetExt.insert(TEXT("exe"));
        m_setTargetExt.insert(TEXT("dll"));
        m_setTargetExt.insert(TEXT("elf"));
        m_setTargetExt.insert(TEXT("so"));
        m_setTargetExt.insert(TEXT("vbs"));
        m_setTargetExt.insert(TEXT("vba"));
        m_setTargetExt.insert(TEXT("doc"));
        m_setTargetExt.insert(TEXT("docx"));
        m_setTargetExt.insert(TEXT("xls"));
        m_setTargetExt.insert(TEXT("xlsx"));
        m_setTargetExt.insert(TEXT("ppt"));
        m_setTargetExt.insert(TEXT("pptx"));
        m_setTargetExt.insert(TEXT("rtf"));
        m_setTargetExt.insert(TEXT("pdf"));
        m_setTargetExt.insert(TEXT("img"));
        m_setTargetExt.insert(TEXT("jpg"));
        m_setTargetExt.insert(TEXT("jpeg"));
        m_setTargetExt.insert(TEXT("apk"));
        m_setTargetExt.insert(TEXT("unkown"));
        m_setTargetExt.insert(TEXT("js"));
        m_setTargetExt.insert(TEXT("zip"));
        m_setTargetExt.insert(TEXT("rar"));
        m_setTargetExt.insert(TEXT("iso"));
        m_setTargetExt.insert(TEXT("jar"));
        m_setTargetExt.insert(TEXT("gz"));
        m_setTargetExt.insert(TEXT("cab"));
        m_setTargetExt.insert(TEXT("lnk"));
        m_setTargetExt.insert(TEXT("hta"));
		
        core::WriteJsonToFile(this, m_strJsonFile);
	}
}

CConfig::~CConfig()
{
    core::WriteJsonToFile(this, m_strJsonFile);
}

void CConfig::AddExt(std::tstring strExt)
{
    m_setTargetExt.insert(strExt);
}
