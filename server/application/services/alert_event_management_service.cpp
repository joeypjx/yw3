#include "alert_event_management_service.hpp"
#include "../dto/dto_converter.hpp"
#include "../dto/alert_event_dto.hpp"
#include "../../domain/entities/alert_event.hpp"
#include "../../domain/entities/alert_rule.hpp"
#include <iostream>

namespace monitoring::application {

AlertEventManagementService::AlertEventManagementService(
    std::shared_ptr<domain::IAlertEventRepository> alertEventRepository
)
    : alertEventRepository_(alertEventRepository)
{
}

AlertEventListResponseDTO AlertEventManagementService::getAllAlertEvents() {
    AlertEventListResponseDTO response;
    
    try {
        auto events = alertEventRepository_->findAll();
        
        response.code = 0;
        response.message = "Success";
        response.events.clear();
        response.total = events.size();
        
        for (const auto& event : events) {
            response.events.push_back(DTOConverter::toAlertEventDTO(event));
        }
        
        std::cout << "[AlertEventManagementService] Retrieved " << events.size() 
                  << " alert events" << std::endl;
    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        response.events = {};
        response.total = 0;
        std::cerr << "[AlertEventManagementService] Error getting all alert events: " << e.what() << std::endl;
    }
    
    return response;
}

AlertEventDetailResponseDTO AlertEventManagementService::getAlertEventById(int64_t eventId) {
    AlertEventDetailResponseDTO response;
    
    try {
        auto eventOpt = alertEventRepository_->findById(eventId);
        if (!eventOpt.has_value()) {
            response.code = 404;
            response.message = "Alert event not found";
            return response;
        }
        
        response.code = 0;
        response.message = "Success";
        response.event = DTOConverter::toAlertEventDTO(eventOpt.value());
        
        std::cout << "[AlertEventManagementService] Retrieved alert event: " << eventId << std::endl;
    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        std::cerr << "[AlertEventManagementService] Error getting alert event " << eventId << ": " << e.what() << std::endl;
    }
    
    return response;
}

AlertEventListResponseDTO AlertEventManagementService::getAlertEventsByNode(const std::string& nodeId) {
    AlertEventListResponseDTO response;
    
    try {
        auto events = alertEventRepository_->findByNode(nodeId);
        
        response.code = 0;
        response.message = "Success";
        response.events.clear();
        response.total = events.size();
        
        for (const auto& event : events) {
            response.events.push_back(DTOConverter::toAlertEventDTO(event));
        }
        
        std::cout << "[AlertEventManagementService] Retrieved " << events.size() 
                  << " alert events for node: " << nodeId << std::endl;
    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        response.events = {};
        response.total = 0;
        std::cerr << "[AlertEventManagementService] Error getting alert events for node " 
                  << nodeId << ": " << e.what() << std::endl;
    }
    
    return response;
}

AlertEventListResponseDTO AlertEventManagementService::getAlertEventsByRule(int32_t ruleId) {
    AlertEventListResponseDTO response;
    
    try {
        auto events = alertEventRepository_->findByRule(ruleId);
        
        response.code = 0;
        response.message = "Success";
        response.events.clear();
        response.total = events.size();
        
        for (const auto& event : events) {
            response.events.push_back(DTOConverter::toAlertEventDTO(event));
        }
        
        std::cout << "[AlertEventManagementService] Retrieved " << events.size() 
                  << " alert events for rule: " << ruleId << std::endl;
    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        response.events = {};
        response.total = 0;
        std::cerr << "[AlertEventManagementService] Error getting alert events for rule " 
                  << ruleId << ": " << e.what() << std::endl;
    }
    
    return response;
}

AlertEventListResponseDTO AlertEventManagementService::getActiveAlertEvents() {
    AlertEventListResponseDTO response;
    
    try {
        auto events = alertEventRepository_->findAllActive();
        
        response.code = 0;
        response.message = "Success";
        response.events.clear();
        response.total = events.size();
        
        for (const auto& event : events) {
            response.events.push_back(DTOConverter::toAlertEventDTO(event));
        }
        
        std::cout << "[AlertEventManagementService] Retrieved " << events.size() 
                  << " active alert events" << std::endl;
    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        response.events = {};
        response.total = 0;
        std::cerr << "[AlertEventManagementService] Error getting active alert events: " << e.what() << std::endl;
    }
    
    return response;
}

AlertEventListResponseDTO AlertEventManagementService::getAlertEventsByStatus(const std::string& status) {
    AlertEventListResponseDTO response;
    
    try {
        // 转换字符串状态为枚举
        domain::AlertEvent::Status eventStatus;
        if (status == "FIRING") {
            eventStatus = domain::AlertEvent::Status::FIRING;
        } else if (status == "ACKNOWLEDGED") {
            eventStatus = domain::AlertEvent::Status::ACKNOWLEDGED;
        } else if (status == "RESOLVED") {
            eventStatus = domain::AlertEvent::Status::RESOLVED;
        } else {
            response.code = 400;
            response.message = "Invalid status: " + status;
            response.events = {};
            response.total = 0;
            return response;
        }
        
        auto events = alertEventRepository_->findByStatus(eventStatus);
        
        response.code = 0;
        response.message = "Success";
        response.events.clear();
        response.total = events.size();
        
        for (const auto& event : events) {
            response.events.push_back(DTOConverter::toAlertEventDTO(event));
        }
        
        std::cout << "[AlertEventManagementService] Retrieved " << events.size() 
                  << " alert events with status: " << status << std::endl;
    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        response.events = {};
        response.total = 0;
        std::cerr << "[AlertEventManagementService] Error getting alert events by status " 
                  << status << ": " << e.what() << std::endl;
    }
    
    return response;
}

CommonResponseDTO AlertEventManagementService::acknowledgeAlertEvent(int64_t eventId, const std::string& operatorId) {
    CommonResponseDTO response;
    
    try {
        auto eventOpt = alertEventRepository_->findById(eventId);
        if (!eventOpt.has_value()) {
            response.code = 404;
            response.message = "Alert event not found";
            return response;
        }
        
        auto event = eventOpt.value();
        event.acknowledge(operatorId);
        alertEventRepository_->save(event);
        
        response.code = 0;
        response.message = "Alert event acknowledged successfully";
        
        std::cout << "[AlertEventManagementService] Acknowledged alert event " << eventId 
                  << " by operator " << operatorId << std::endl;
    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        std::cerr << "[AlertEventManagementService] Error acknowledging alert event " 
                  << eventId << ": " << e.what() << std::endl;
    }
    
    return response;
}

CommonResponseDTO AlertEventManagementService::resolveAlertEvent(int64_t eventId) {
    CommonResponseDTO response;
    
    try {
        auto eventOpt = alertEventRepository_->findById(eventId);
        if (!eventOpt.has_value()) {
            response.code = 404;
            response.message = "Alert event not found";
            return response;
        }
        
        auto event = eventOpt.value();
        event.resolve();
        alertEventRepository_->save(event);
        
        response.code = 0;
        response.message = "Alert event resolved successfully";
        
        std::cout << "[AlertEventManagementService] Resolved alert event " << eventId << std::endl;
    } catch (const std::exception& e) {
        response.code = 500;
        response.message = "Internal error: " + std::string(e.what());
        std::cerr << "[AlertEventManagementService] Error resolving alert event " 
                  << eventId << ": " << e.what() << std::endl;
    }
    
    return response;
}

} // namespace monitoring::application
