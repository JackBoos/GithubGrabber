#pragma once

#include "commonData.h"
#include <string>
#include <vector>
#include <xlsxio_version.h>
#include <xlsxio_write.h>

class excelExecuter {
public:
  excelExecuter();
  ~excelExecuter();

  static excelExecuter &GetInstance();

  bool WriteToFile(const char *filePath, const ItemsDataList &dataList,
                   const ConditionList &filterConditions);
  bool WriteBase(const char *filePath, const ItemsDataList &dataList);

private:
  void TraverseHeader(const OneItemDataList &dataList,
                      const xlsxiowriter writer,
                      const std::vector<std::string> &filter);
  void TraverseSubHeader(const void *dataList, const xlsxiowriter writer,
                         const std::vector<std::string> &filter,
                         const char *originName);
  void TraverseData(const OneItemDataList &dataList, const xlsxiowriter writer,
                    const std::vector<std::string> &filter);
  void TraverseSubData(const void *dataList, const xlsxiowriter writer,
                       const std::vector<std::string> &filter,
                       const char *originName);
  std::string GetIteamName(const ItemData &dataList, const char *prevString);
  std::string GetFliterName(const Condition &cdt,
                            const char *prevString = nullptr);

private:
  static excelExecuter *_ptr;
};
