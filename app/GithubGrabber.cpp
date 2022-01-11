// GithubGrabber.cpp : Defines the entry point for the application.
//

#include <iostream>
#include "grabber.h"

using namespace std;

int main()
{
    grabber gb("https://api.github.com/search/", "JackBoosY");
    gb.Init();

    ConditionList cdtList;
    cdtList.push_back("type:issue");
    cdtList.push_back("state:open");
    cdtList.push_back("label:\"category:port-bug\"");
    cdtList.push_back("repo:microsoft/vcpkg");
    cdtList.push_back("sort:created");
    gb.GetData(SearchCondition{ SearchType::SEARCHTYPE_ISSUE, SEARCHNAME_ISSUE }, cdtList, "result.xlsx", 50);

	return 0;
}
