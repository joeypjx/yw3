#ifndef PRESENTATION_CONTROLLERS_JSON_UTILS_HPP
#define PRESENTATION_CONTROLLERS_JSON_UTILS_HPP

#include "../../application/dto/heartbeat_dto.hpp"
#include "../../application/dto/resource_dto.hpp"
#include <string>
#include <stdexcept>

// 注意：这里使用 nlohmann/json 库
// 需要在CMakeLists.txt中添加对应的依赖
#include "json.hpp"

namespace monitoring::presentation {

/**
 * JsonUtils - JSON序列化/反序列化工具类
 *
 * 职责：
 * 1. 将JSON字符串反序列化为DTO对象
 * 2. 将DTO对象序列化为JSON字符串
 *
 * 注意：这个类位于表现层，负责处理HTTP传输格式
 * 实际使用时需要引入JSON库（如nlohmann/json）
 */
class JsonUtils {
public:
    /**
     * 解析心跳请求JSON
     *
     * @param jsonStr JSON字符串
     * @return HeartbeatRequestDTO对象
     * @throws std::runtime_error 如果JSON格式不正确
     */
    static application::HeartbeatRequestDTO parseHeartbeatRequest(const std::string& jsonStr);

    /**
     * 解析资源上报请求JSON
     *
     * @param jsonStr JSON字符串
     * @return ResourceReportRequestDTO对象
     * @throws std::runtime_error 如果JSON格式不正确
     */
    static application::ResourceReportRequestDTO parseResourceRequest(const std::string& jsonStr);

    /**
     * 序列化心跳响应为JSON
     *
     * @param dto HeartbeatResponseDTO对象
     * @return JSON字符串
     */
    static std::string serializeHeartbeatResponse(const application::HeartbeatResponseDTO& dto);

    /**
     * 序列化资源上报响应为JSON
     *
     * @param dto ResourceReportResponseDTO对象
     * @return JSON字符串
     */
    static std::string serializeResourceResponse(const application::ResourceReportResponseDTO& dto);
};

} // namespace monitoring::presentation

#endif // PRESENTATION_CONTROLLERS_JSON_UTILS_HPP
