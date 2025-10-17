cmake_minimum_required(VERSION 3.15)

project(MonitoringAgent LANGUAGES CXX)

# 设置C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Agent executable
set(AGENT_SOURCES
    agent_main.cpp
    collector/collector.cpp
    reporter/reporter.cpp
)

add_executable(monitoring_agent ${AGENT_SOURCES})

# Include directories
target_include_directories(monitoring_agent PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Find and link libcurl
find_package(CURL REQUIRED)
target_link_libraries(monitoring_agent PRIVATE CURL::libcurl)

# Link with pthread for threading support
find_package(Threads REQUIRED)
target_link_libraries(monitoring_agent PRIVATE Threads::Threads)
