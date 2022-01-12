#include "urlParser.h"
#include <curl/curl.h>
#include <regex>
#include "commonData.h"

urlParser::urlParser(const char* url)
{
    if (url && *url)
    {
        m_currUrlBase.append(url);
        if (m_currUrlBase.at(m_currUrlBase.size() - 1) != '/') 
            m_currUrlBase.append("/");
    }
}

urlParser::~urlParser()
{
}

bool urlParser::Init(const char* username, const char* token)
{
    if (m_currUrlBase.empty())
        return false;
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, GITHUB_REQUIRED_USER_AGENT_HEADER);
    headers = curl_slist_append(headers, GITHUB_REQUIRED_USER_AGENT_HEADER);
    curl = curl_easy_init();
    if (curl)
    {
        if (username && *username)
        {
            std::string strToken = username;
            strToken = username;
            if (token && *token)
            {
                strToken += ":";
                strToken += token;
                m_token = strToken;
            }
            curl_easy_setopt(curl, CURLOPT_USERPWD, m_token.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, m_currUrlBase.c_str());
        res = curl_easy_perform(curl);
        if (res != 0) {

            curl_slist_free_all(headers);
        }
        curl_easy_cleanup(curl);
        return res == 0;
    }

    return true;
}

bool urlParser::GetData(const char* extUrl, std::string& outData)
{
    outData.clear();
    if (!extUrl || !(*extUrl))
        return false;
    std::string subUrl = m_currUrlBase;
    std::string realSubUrl = extUrl;
    realSubUrl = std::regex_replace(realSubUrl, std::regex(" "), "%20");
    realSubUrl = std::regex_replace(realSubUrl, std::regex("\""), "%22");
    subUrl.append(realSubUrl);

    CURL *curl;
    CURLcode res;

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, GITHUB_REQUIRED_USER_AGENT_HEADER);
    headers = curl_slist_append(headers, GITHUB_REQUIRED_USER_AGENT_HEADER);
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, subUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outData);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, urlParser::AppendData);
        res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return res == 0;
    }
    return true;
}

bool urlParser::PostData(const char* extUrl, const std::string& inData, const char* customRequest)
{
    if (!extUrl || !(*extUrl))
        return false;

    if (m_token.empty())
        return false;

    std::string subUrl = m_currUrlBase;
    std::string realSubUrl = extUrl;
    realSubUrl = std::regex_replace(realSubUrl, std::regex(" "), "%20");
    realSubUrl = std::regex_replace(realSubUrl, std::regex("\""), "%22");
    subUrl.append(realSubUrl);

    CURL *curl;
    CURLcode res;

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, GITHUB_REQUIRED_USER_AGENT_HEADER);
    headers = curl_slist_append(headers, GITHUB_REQUIRED_USER_AGENT_HEADER);
    curl = curl_easy_init();
    if (curl)
    {
        if (customRequest && *customRequest)
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, customRequest);
        curl_easy_setopt(curl, CURLOPT_USERPWD, m_token.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, subUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, inData.c_str());    // 指定post内容
        res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return res == 0;
    }

    return res == 0;
}

size_t urlParser::AppendData(void *ptr, size_t size, size_t nmemb, void *stream)
{
    const char* data = static_cast<const char*>(ptr);
    if (!stream)
    {
        return 0;
    }
    std::string* pOut = static_cast<std::string*>(stream);

    pOut->append(data, size * nmemb);

    return size * nmemb;
}