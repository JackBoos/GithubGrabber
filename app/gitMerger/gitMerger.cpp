#include "grabber.h"
#include "gitOperator.h"
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Please enter PR number first." << std::endl;
        return 1;
    }

    std::cout << "Do you want to merge PR " << argv[1] << " to master?" << std::endl << "Y:n" << std::endl;
    std::string choose;
    std::cin >> choose;
    if (choose != "Y" && choose != "y")
    {
        std::cout << "Canceled." << std::endl;
        return 0;
    }

    GithubGrabber::grabber gb("https://api.github.com/repos/microsoft/vcpkg/pulls/");
    if (!gb.Init())
    {

        return 1;
    }

    ItemsDataList list;
    if (gb.GetPerIssueOrPR(argv[1], &list))
    {
        std::cout << std::endl;
        auto& data = list.begin();
        auto head = find(data->begin(), data->end(), "head");
        auto prRepo = find(((ItemsDataList*)(head->value))->begin()->begin(), ((ItemsDataList*)(head->value))->begin()->end(), "repo");
        auto prUrl = find(((ItemsDataList*)(prRepo->value))->begin()->begin(), ((ItemsDataList*)(prRepo->value))->begin()->end(), "clone_url");
        auto branch = find(((ItemsDataList*)(head->value))->begin()->begin(), ((ItemsDataList*)(head->value))->begin()->end(), "ref");

        std::string& strRepo = *((std::string*)prUrl->value);
        std::string& strBranch = *((std::string*)branch->value);

        GithubGrabber::gitOperator op;
        if (!op.Init())
            return 1;

        GithubGrabber::gitRemote remotePR, remoteofficial;
        op.AddRemote(strRepo, remotePR);
        op.changeBranch(remotePR, strBranch);
        op.AddRemote("https://github.com/microsoft/vcpkg.git", remoteofficial, remotePR);
        strcpy(remoteofficial.szBranchName, "master");
        op.PullLatestChanges(remotePR, remoteofficial);
        op.PushLatestChanges(remotePR);
    }

    return 0;
}