#pragma once

class CConfig : public core::IFormatterObject
{
	CConfig();
	~CConfig();
	
public:
	std::tstring m_strJsonFile;
	std::set<std::tstring> m_setTargetExt;
	void AddExt(std::tstring strExt);
	
	static CConfig* GetInstance()
	{
		static CConfig instance;
		return &instance;
	}

	void OnSync(core::IFormatter& formatter)
	{
		formatter
			+ core::sPair(TEXT("TargetExt"), m_setTargetExt);
	}
	
};

inline CConfig* Config()
{
	return CConfig::GetInstance();
}