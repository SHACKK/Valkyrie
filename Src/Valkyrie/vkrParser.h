#ifndef __VKRPARSER__
#define __VKRPARSER__

ST_KEY_SET StructFromVkr(std::string ValTempStr);
int vkrParser(const char* fName);

std::vector<std::tstring> GetAllHashByDetection(CString DetectionName);
std::vector<std::tstring> GetAllBenignHashByExt(CString ExtentionName, CString DetectionName);

#endif __VKRPARSER__