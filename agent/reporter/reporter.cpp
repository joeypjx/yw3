#include "reporter.hpp"
#include <curl/curl.h>
#include <iostream>

namespace monitoring::agent {

// libcurl的回调函数，用于接收响应数据
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* response = static_cast<std::string*>(userp);
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

Reporter::Reporter() : curl_(nullptr) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_ = curl_easy_init();
}

Reporter::~Reporter() {
    if (curl_) {
        curl_easy_cleanup(static_cast<CURL*>(curl_));
    }
    curl_global_cleanup();
}

bool Reporter::sendPost(const std::string& url, const std::string& jsonData) {
    if (!curl_) {
        lastError_ = "CURL not initialized";
        return false;
    }

    CURL* curl = static_cast<CURL*>(curl_);
    std::string responseData;

    // 设置URL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    // 设置为POST请求
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    // 设置POST数据
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonData.size());

    // 设置HTTP头：Content-Type: application/json
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // 设置回调函数接收响应
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    // 设置超时（10秒）
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    // 执行请求
    CURLcode res = curl_easy_perform(curl);

    // 清理headers
    curl_slist_free_all(headers);

    // 检查结果
    if (res != CURLE_OK) {
        lastError_ = std::string("curl_easy_perform() failed: ") + curl_easy_strerror(res);
        return false;
    }

    // 检查HTTP状态码
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    if (httpCode != 200) {
        lastError_ = "HTTP error code: " + std::to_string(httpCode);
        std::cerr << "Server response: " << responseData << std::endl;
        return false;
    }

    // 成功
    std::cout << "Successfully sent data to " << url << std::endl;
    std::cout << "Response: " << responseData << std::endl;
    return true;
}

} // namespace monitoring::agent
