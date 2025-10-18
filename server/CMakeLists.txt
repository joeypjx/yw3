cmake_minimum_required(VERSION 3.15)

project(MonitoringServer)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Server executable
set(SERVER_SOURCES
    main.cpp
    # Domain Layer
    domain/entities/server_node.cpp
    domain/entities/metric_snapshot.cpp
    domain/entities/alert_rule.cpp
    domain/entities/alert_event.cpp
    domain/services/alert_expression_evaluation_service.cpp

    # Application Layer
    application/services/ingestion_service.cpp
    application/services/query_service.cpp
    application/services/alerting_orchestrator_service.cpp
    application/services/alert_rule_management_service.cpp
    application/services/alert_event_management_service.cpp

    # Infrastructure Layer (Mock implementations)
    infrastructure/repositories/mock_node_repository.cpp
    infrastructure/repositories/mock_metric_repository.cpp
    infrastructure/repositories/mock_alert_rule_repository.cpp
    infrastructure/repositories/mock_alert_event_repository.cpp
    infrastructure/notifiers/mock_notification_service.cpp

    # Presentation Layer
    presentation/controllers/http_server.cpp
    presentation/controllers/simple_json_parser.cpp
)

add_executable(monitoring_server ${SERVER_SOURCES})

# Include directories
target_include_directories(monitoring_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/domain
    ${CMAKE_CURRENT_SOURCE_DIR}/application
    ${CMAKE_CURRENT_SOURCE_DIR}/infrastructure
    ${CMAKE_CURRENT_SOURCE_DIR}/presentation
    /opt/homebrew/include
)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Link with required libraries
target_link_libraries(monitoring_server PRIVATE 
    pthread
    OpenSSL::SSL
    OpenSSL::Crypto
)
