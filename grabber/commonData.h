#pragma once
#include <string>
#include <list>

#define SEARCHNAME_ISSUE "issues"
#define SEARCHNAME_PR "pr"

#define QUERY_KEYWORD "?q="
#define PAGE_KEYWORD "&page="
#define PER_PARGE_KEYWORD "&per_page="

#define GITHUB_TOTAL_COUNT_NAME "total_count"
#define GITHUB_ITEAM_NAME "items"

#define GITHUB_REQUIRED_USER_AGENT_HEADER "User-Agent: github-grabber-app"
#define GITHUB_REQUIRED_ACCEPT_HEADER "Accept: application/vnd.github.v3+json"

typedef enum _DataType
{
    DATATYPE_UNKNOWN = 0,
    DATATYPE_INT = 1,
    DATATYPE_STRING = 2,
}DataType;

typedef enum _SearchType
{
    SEARCHTYPE_UNKNOWN = 0,
    SEARCHTYPE_ISSUE = 1,
    SEARCHTYPE_PR = 2,
}SearchType;

typedef struct _SearchCondition
{
    SearchType type;
    std::string name;
}SearchCondition;

typedef struct _ItemData
{
    std::string name;
    DataType type;
    void* value;
}ItemData;

typedef std::list<ItemData> OneIteamDataList;
typedef std::list<OneIteamDataList> IteamDataList;

typedef std::list<std::string> ConditionList;
