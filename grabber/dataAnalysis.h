#pragma once
#include "commonData.h"
#include <json/json.h>
#include <string>

class dataAnalysis {
public:
  dataAnalysis();
  ~dataAnalysis();

  static dataAnalysis &GetInstance();

  int GetTotalSize(const std::string &inString, const char *keyword);
  bool ParseData(const std::string &inString, const char *subLable,
                 ItemsDataList &pOut);

private:
  void handleObject(const Json::Value &root, ItemsDataList &pOut);
  void handleArray(const Json::Value &root, ItemsDataList &pOut);
  void handleNormal(const Json::Value &root, ItemData &pOut);

private:
  static dataAnalysis *_ptr;
  std::string *pString;
};
