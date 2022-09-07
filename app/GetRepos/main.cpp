#include "dataAnalysis.h"
#include "excelExecuter.h"
#include "grabber.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

typedef struct _repo_data {
  _repo_data(std::string &name, std::string &page, int num = 0) {
    repo_name = name;
    home_page = page;
    star = num;
  }
  std::string repo_name;
  std::string home_page;
  int star;
} repo_data;

int main(int argv, char **argc) {
  if (argv < 3) {
    std::cout << "please enter parameters: userName vcpkgPath" << std::endl;
    return -1;
  }

  const char *user = argc[1];
  const char *vcpkg_root = argc[2];

  std::map<std::string, repo_data> github_repos;
  std::string ports = vcpkg_root;
  ports.append("/ports");

  char *pBuffer = new char[2048];
  for (auto &port : std::experimental::filesystem::directory_iterator(ports)) {
    std::string portfile = port.path().string();
    portfile.append("/portfile.cmake");

    std::fstream ss;
    ss.open(portfile);
    if (!ss.is_open())
      continue;

    std::string content;
    while (ss.read(pBuffer, 2048)) {
      content.append(pBuffer);
    }
    content.append(pBuffer);

    ss.close();

    auto begin = content.find("vcpkg_from_github");
    if (begin == std::string::npos)
      continue;

    auto repo_begin = content.find("REPO", begin);

    if (repo_begin == std::string::npos)
      continue;

    auto repo_end = content.find("\n", repo_begin);

    std::string repo_name = content.substr(
        repo_begin + strlen("REPO"), repo_end - repo_begin - strlen("REPO"));

    while (repo_name[0] == ' ')
      repo_name.erase(0, 1);

    if (repo_name[0] == '"' && repo_name[repo_name.length() - 1] == '"') {
      repo_name.erase(0, 1);
      repo_name.erase(repo_name.length() - 1, 1);
    }

    github_repos.insert(std::make_pair(
        port.path().filename().string(),
        repo_data(repo_name, "https://github.com/" + repo_name, 0)));
  }

  delete[] pBuffer;

  char token[64] = {0};
  std::fstream ssToken;
  ssToken.open("token.txt");
  if (!ssToken.is_open()) {
    std::cout << "open token file 'token.txt' failed:" << strerror(errno)
              << std::endl;
    return -1;
  }

  ssToken.read(token, 63);
  ssToken.close();

  GithubGrabber::grabber gb("https://api.github.com/repos/", user, token);
  gb.Init();

  for (auto &one_port : github_repos) {
    std::string data;
    if (!gb.ExecuteBaseGet(one_port.second.repo_name, data))
      continue;

    ItemsDataList data_list;
    if (!dataAnalysis::GetInstance().ParseData(data, nullptr, data_list))
      continue;

    if (!data_list.size())
      continue;

    auto &iter = std::find(data_list.begin()->begin(), data_list.begin()->end(),
                           "stargazers_count");

    if (iter == data_list.begin()->end())
      continue;

    one_port.second.star = *(static_cast<int *>(iter->value));
  }

  ItemsDataList result_list;
  for (auto &one_data : github_repos) {

    if (one_data.second.star < 1000)
      continue;
    OneItemDataList data;

    data.push_back({"name", DATATYPE_STRING, (void *)&one_data.first});
    data.push_back(
        {"repo", DATATYPE_STRING, (void *)&one_data.second.repo_name});
    data.push_back(
        {"home_page", DATATYPE_STRING, (void *)&one_data.second.home_page});
    data.push_back({"star", DATATYPE_INT, (void *)&one_data.second.star});

    result_list.push_back(data);
  }

  excelExecuter::GetInstance().WriteToFile("result.xlsx", result_list,
                                           ConditionList());

  return 0;
}