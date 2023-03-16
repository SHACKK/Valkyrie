#pragma once
#include "../../../zlib/Inc/zlib/zip.h"

class CValkyrieZip
{
	zipFile zf;
	zlib_filefunc64_32_def m_zfInfo;
	
public:
	CValkyrieZip();
	~CValkyrieZip();
	
	bool Create(std::tstring strTargetDirectory);
	bool Close();
	bool Add(std::tstring strFileName, std::vector<BYTE>& vecBinary, std::tstring strPasswd = TEXT(""));
};
