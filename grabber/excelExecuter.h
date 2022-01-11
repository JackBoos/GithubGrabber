#pragma once

#include "commonData.h"

class excelExecuter
{
public:
    excelExecuter();
    ~excelExecuter();

    static excelExecuter& GetInstance();

    bool WriteToFile(const char* filePath, const IteamDataList& dataList);

private:
    static excelExecuter* _ptr;
};
