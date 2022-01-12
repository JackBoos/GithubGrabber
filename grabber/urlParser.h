#pragma once

#include <string>

class urlParser
{
public:
    urlParser(const char* url = nullptr);
    ~urlParser();

    bool Init(const char* username = nullptr, const char* token = nullptr);

    void ResetBaseUrl(const char* baseurl);

    bool GetData(const char* extUrl, std::string& outData);

    bool PostData(const char* extUrl, const std::string& inData = std::string(), const char* customRequest = nullptr);

private:
    static size_t AppendData(void *ptr, size_t size, size_t nmemb, void *stream);

private:
    std::string m_currUrlBase;
    std::string m_token;
};
