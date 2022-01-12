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

#define GITHUB_SEARCH_BASE_URL "https://api.github.com/search/"
#define GITHUB_SEARCH_OPERATE_URL "https://api.github.com/"

#define GITHUB_REPOS_NAME "repos/"
#define GITHUB_ISSUES_NAME "/issues/"
#define GITHUB_PRS_NAME "/pulls/"
#define GITHUB_COMMENTS_NAME "/comments"

#define GITHUB_REQUIRED_USER_AGENT_HEADER "User-Agent: github-grabber-app"
#define GITHUB_REQUIRED_ACCEPT_HEADER "Accept: application/vnd.github.v3+json"

#define GITHUB_OP_PATCH "PATCH"

#define GITHUB_BODY_START "{\"body\":\""
#define GITHUB_BODY_END "\"}"
#define GITHUB_CLOSE_ISSUE_PR_BODY "{\"state\":\"closed\"}"

typedef enum _DataType
{
    DATATYPE_UNKNOWN = 0,
    DATATYPE_INT = 1,
    DATATYPE_STRING = 2,
    DATATYPE_BOOLEAN = 3,
}DataType;

typedef enum _SearchType
{
    SEARCHTYPE_UNKNOWN = 0,
    SEARCHTYPE_ISSUE = 1,
    SEARCHTYPE_PR = 2,
}SearchType;

typedef enum _ExecuteType
{
    EXECUTE_TYPE_UNKNOWN = 0,
    EXECUTE_TYPE_ADD_COMMENT = 1,
    EXECUTE_TYPE_CLOSE_ISSUE_PR = 2,
}ExecuteType;

typedef enum _ExecuteOpDest
{
    EXECUTE_OP_TO_NONE = 0,
    EXECUTE_OP_TO_ASSIGNER = 1,
    EXECUTE_OP_TO_AUTHOR = 2,
}ExecuteOpDest;

typedef struct _SearchCondition
{
    SearchType type;
    std::string name;
}SearchCondition;

typedef struct _ItemData
{
    bool operator == (const std::string& dst) const
    {
        return (name == dst);
    }
    std::string name;
    DataType type;
    void* value;
}ItemData;

typedef struct _ExecuteData
{
    ExecuteType type;
    unsigned int issue_pr_id;
    ExecuteOpDest dst;
    std::string comment;
}ExecuteData;

typedef std::list<ItemData> OneIteamDataList;
typedef std::list<OneIteamDataList> IteamDataList;

typedef std::list<std::string> ConditionList;
