#ifndef AGENT_REPORTER_HPP
#define AGENT_REPORTER_HPP

#include <string>

namespace monitoring::agent {

/**
 * Reporter - HTTP上报器
 * 负责通过HTTP POST发送JSON数据到服务器
 *
 * 注意：需要libcurl库支持
 * Ubuntu/Debian: apt-get install libcurl4-openssl-dev
 * CentOS/RHEL: yum install libcurl-devel
 */
class Reporter {
public:
    Reporter();
    ~Reporter();

    /**
     * 发送HTTP POST请求
     *
     * @param url 目标URL
     * @param jsonData JSON数据
     * @return 是否发送成功
     */
    bool sendPost(const std::string& url, const std::string& jsonData);

    /**
     * 获取最后一次错误信息
     */
    std::string getLastError() const { return lastError_; }

private:
    std::string lastError_;
    void* curl_;  // CURL* 指针，避免头文件依赖
};

} // namespace monitoring::agent

#endif // AGENT_REPORTER_HPP
