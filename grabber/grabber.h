#pragma once
#include "commonData.h"
#include "urlParser.h"
#include <string>

namespace GithubGrabber {
class grabber {
public:
  grabber(const char *baseUrl, const char *username = nullptr,
          const char *token = nullptr);
  ~grabber();

  bool Init();

  void ResetBaseUrl(const char *baseurl);

  bool GetPerIssueOrPR(const std::string &number, ItemsDataList *outDataList);
  bool GetData(SearchCondition &search, const ConditionList &conditions,
               const char *outFile, unsigned int onePageCount,
               const ConditionList &filterConditions = ConditionList(),
               ItemsDataList *outDataList = nullptr, bool bSave = true);

  bool Execute(const std::string &repo, SearchType type,
               const ExecuteData &data);

  bool ExecuteBaseGet(const std::string &url, std::string &data);

  bool ExecuteBasePost(const std::string &url, std::string &data);

private:
  bool bInit;
  std::string m_baseUrl;
  std::string m_usr;
  std::string m_token;
  urlParser *m_parser;
};
} // namespace GithubGrabber
