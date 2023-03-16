#include "pch.h"
#include "Updator.h"

void CUpdator::Descript(ST_UPDATE_INFO& outInfo)
{
	outInfo.strProductCode = TEXT("VLKR");
	outInfo.strProductDir = core::ExtractDirectory(core::GetFileName());
	outInfo.vecExeFiles.push_back(TEXT("Valkyrie.exe"));
}
