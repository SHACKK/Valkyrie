#include "pch.h"

std::ifstream readFile;
std::vector<ST_KEY_SET> vecParsed;


ST_KEY_SET StructFromVkr(std::string ValTempStr)
{
    std::istringstream iss(ValTempStr);
    std::string stringBuf;
    std::vector<std::string> vecVal;

    for (size_t j = 0; j < sizeof(ST_KEY_SET); j++)
    {
        getline(iss, stringBuf, ',');
        vecVal.push_back(stringBuf);
    }

    ST_KEY_SET stKeySet;
    stKeySet.date = vecVal[0];
    stKeySet.ext = vecVal[1];
    stKeySet.detection = vecVal[2];
    stKeySet.detection_name = vecVal[3];
    stKeySet.engine_ver = vecVal[4];
    stKeySet.elapsed_time = vecVal[5];
    stKeySet.file_path = vecVal[6];
    stKeySet.sha256 = vecVal[7];
    stKeySet.vt_positives = vecVal[8];
    stKeySet.vt_total = vecVal[9];
    stKeySet.vt_keyword = vecVal[10];

    return stKeySet;
}


int vkrParser(const char* fName)
{
    readFile.open(fName);

    if (readFile.is_open())
    {
        if (readFile.good())
        {
            std::string tmpKey;
            std::getline(readFile, tmpKey);
            size_t nPos = tmpKey.find("elapsed_time");

            if (nPos != std::string::npos)
            {   /* Prevent runtime error */    }

            while (!readFile.eof())
            {
                std::string valTemp;
                std::getline(readFile, valTemp);
                vecParsed.push_back(StructFromVkr(valTemp));
            }
            readFile.close();
        }
        else if (readFile.fail())   /* Internal file contents load failure */
        {   return 1;   }
        else                        /* File open failure */
        {   return 2;   }
    }
    else                            /* Failed to open file by Unkown error */
    {   return 3;   }
    return 0;
}


std::vector<std::tstring> GetAllHashByDetection(CString DetectionName)
{
    std::vector<std::tstring> vecHash;

    char* detectionN = new char[DetectionName.GetLength()];
    strcpy(detectionN, CT2A(DetectionName));

    for (auto iter : vecParsed)
    {
        if (!strcmp(iter.detection.c_str(), detectionN))
        {
            size_t nSize = 0;
            TCHAR strFileName[MAX_PATH + 1] = { 0, };

            mbstowcs_s(&nSize, strFileName, iter.file_path.c_str(), MAX_PATH);
            vecHash.push_back(strFileName);
        }
    }
    return vecHash;
}


std::vector<std::tstring> GetAllBenignHashByExt(CString ExtentionName, CString DetectionName)
{
    std::vector<std::tstring> vecHash;

    char* extentionN = new char[ExtentionName.GetLength()];
    char* detectionN = new char[DetectionName.GetLength()];

    strcpy(extentionN, CT2A(ExtentionName));
    strcpy(detectionN, CT2A(DetectionName));

    for (auto iter : vecParsed)
    {
        std::for_each
        (
            iter.ext.begin(),
            iter.ext.end(),
            [](char& szLowerExt)
            {   szLowerExt = ::tolower(szLowerExt);     }
        );

        if (!strcmp(iter.ext.c_str(), extentionN))
        {
            if (!strcmp(iter.detection.c_str(), detectionN))
            {
                size_t nSize = 0;
                TCHAR strFileName[MAX_PATH] = {0, };

                mbstowcs_s(&nSize, strFileName, iter.file_path.c_str(), MAX_PATH);
                vecHash.push_back(strFileName);
            }
        }
    }
    return vecHash;
}