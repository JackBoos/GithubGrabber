#include "gitOperator.h"
#include "grabber.h"
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Please enter PR number first." << std::endl;
    return 1;
  }

  std::cout << "Do you want to merge PR " << argv[1] << " to master?"
            << std::endl
            << "Y:n" << std::endl;
  std::string choose;
  std::cin >> choose;
  if (choose != "Y" && choose != "y") {
    std::cout << "Canceled." << std::endl;
    return 0;
  }

  GithubGrabber::grabber gb(
      "https://api.github.com/repos/microsoft/vcpkg/pulls/");
  if (!gb.Init()) {
    std::cout << "Init grabber failed." << std::endl;
    return 1;
  }

  ItemsDataList list;
  if (gb.GetPerIssueOrPR(argv[1], &list)) {
    std::cout << std::endl;
    auto &data = list.begin();
    auto head = find(data->begin(), data->end(), "head");
    if (head == data->end()) {
      std::cout << "Failed to parse data: no head." << std::endl;
      return 1;
    }
    auto prRepo =
        find(((ItemsDataList *)(head->value))->begin()->begin(),
             ((ItemsDataList *)(head->value))->begin()->end(), "repo");
    if (prRepo == ((ItemsDataList *)(head->value))->begin()->end()) {
      std::cout << "Failed to parse data: no repo." << std::endl;
      return 1;
    }
    auto prUrl =
        find(((ItemsDataList *)(prRepo->value))->begin()->begin(),
             ((ItemsDataList *)(prRepo->value))->begin()->end(), "clone_url");
    if (prUrl == ((ItemsDataList *)(prRepo->value))->begin()->end()) {
      std::cout << "Failed to parse data: no clone_url." << std::endl;
      return 1;
    }
    auto branch = find(((ItemsDataList *)(head->value))->begin()->begin(),
                       ((ItemsDataList *)(head->value))->begin()->end(), "ref");
    if (branch == ((ItemsDataList *)(head->value))->begin()->end()) {
      std::cout << "Failed to parse data: no ref." << std::endl;
      return 1;
    }

    std::string &strRepo = *((std::string *)prUrl->value);
    std::string &strBranch = *((std::string *)branch->value);

    if (strRepo.empty()) {
      std::cout << "Failed to parse data: repo is null" << std::endl;
      return 1;
    }
    if (strBranch.empty()) {
      std::cout << "Failed to parse data: branch name is null" << std::endl;
      return 1;
    }

    std::cout << "Repo: " << strRepo << " branch: " << strBranch << std::endl;
    GithubGrabber::gitOperator op;
    if (!op.Init()) {
      std::cout << "Failed to get git path." << std::endl;
      return 1;
    }

    GithubGrabber::gitRemote remotePR, remoteofficial;
    std::cout << "Clone " << strRepo << " ..." << std::endl;
    if (!op.AddRemote(strRepo, remotePR)) {
      return 1;
    }
    if (strBranch != remotePR.szBranchName) {
      std::cout << "Switch branch to " << strBranch << " ..." << std::endl;
      if (!op.changeBranch(remotePR, strBranch)) {
        return 1;
      }
    }
    std::cout << "Add official repo..." << std::endl;
    if (!op.AddRemote("https://github.com/microsoft/vcpkg.git", remoteofficial,
                      remotePR)) {
      return 1;
    }
    strcpy(remoteofficial.szBranchName, "master");
    std::cout << "Push changes ..." << std::endl;
    if (!op.PullLatestChanges(remotePR, remoteofficial)) {
      return 1;
    }
    return op.PushLatestChanges(remotePR) == true ? 0 : 1;
  } else {
    std::cout << "Failed to get PR " << argv[1] << " data" << std::endl;
  }

  return 0;
}