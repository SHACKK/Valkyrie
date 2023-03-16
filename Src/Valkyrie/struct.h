#pragma once
struct ST_KEY_SET
{
    std::string date;
    std::string ext;
    std::string detection;
    std::string detection_name;
    std::string engine_ver;
    std::string elapsed_time;
    std::string file_path;
    std::string sha256;
    std::string vt_positives;
    std::string vt_total;
    std::string vt_keyword;

    void PrintData()
    {
        printf("%s  ", date.c_str());
        printf("%s  ", ext.c_str());
        printf("%s  ", detection.c_str());
        printf("%s  ", detection_name.c_str());
        printf("%s  ", engine_ver.c_str());
        printf("%s  ", elapsed_time.c_str());
        printf("%s  ", file_path.c_str());
        printf("%s  ", sha256.c_str());
        printf("%s  ", vt_positives.c_str());
        printf("%s  ", vt_total.c_str());
        printf("%s  ", vt_keyword.c_str());
    }
};