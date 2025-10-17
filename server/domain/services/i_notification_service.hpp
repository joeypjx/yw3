#ifndef DOMAIN_SERVICES_I_NOTIFICATION_SERVICE_HPP
#define DOMAIN_SERVICES_I_NOTIFICATION_SERVICE_HPP

#include <string>
#include "../entities/alert_event.hpp"

namespace monitoring::domain {

/**
 * INotificationService - 通知服务接口
 * 定义发送告警通知的能力
 * 这是一个领域服务接口，供应用层使用
 */
class INotificationService {
public:
    virtual ~INotificationService() = default;

    /**
     * 发送告警触发通知
     * @param event 告警事件
     */
    virtual void sendAlertTriggered(const AlertEvent& event) = 0;

    /**
     * 发送告警解决通知
     * @param event 告警事件
     */
    virtual void sendAlertResolved(const AlertEvent& event) = 0;

    /**
     * 发送告警认知通知
     * @param event 告警事件
     */
    virtual void sendAlertAcknowledged(const AlertEvent& event) = 0;
};

} // namespace monitoring::domain

#endif // DOMAIN_SERVICES_I_NOTIFICATION_SERVICE_HPP
