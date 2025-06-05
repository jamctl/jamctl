cmake_minimum_required(VERSION 3.27)
project(jamd VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_definitions(-DUSE_REFLECTION)

find_package(Boost CONFIG REQUIRED COMPONENTS filesystem process)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(Drogon REQUIRED)
find_package(spdlog REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(OpenSSL REQUIRED)
add_executable(jamd main.cpp
        modules/backup/backup.cpp
        modules/backup/backup.h
        modules/manager/InstanceManager/InstanceManager.cpp
        modules/manager/InstanceManager/InstanceManager.h
        modules/manager/InstanceManager/SingleServer.hpp
        utils/Singleton.h
        controllers/Instances.h
        controllers/User.h
        controllers/Instances.cpp
        controllers/User.cpp
        modules/manager/ConfigFileManager/ConfigFileManager.cpp
        modules/manager/ConfigFileManager/ConfigFileManager.h
        plugins/log/RequestLogger.h
        plugins/log/RequestLogger.cpp
        ext/extFuns.h
        ext/marcos.h
        ext/alias.h
        structs/api/LaunchResult.h
        structs/api.h
        structs/config/ServerConfig.h
        structs/config.h
        plugins/autoAddHeaders/AutoAddHeaders.cpp
        plugins/autoAddHeaders/AutoAddHeaders.h
        ext/autoSerialization.h
        ext/styledORM.h
        structs/config/JamdConfig.h
)

if (MSVC)
    target_compile_options(jamd PRIVATE /experimental:module /D_WIN32_WINNT=0x0A00)
else ()
    target_compile_options(jamd PRIVATE -fmodules-ts)
endif ()

target_link_libraries(jamd PRIVATE Boost::filesystem Boost::process PostgreSQL::PostgreSQL yaml-cpp::yaml-cpp JsonCpp::JsonCpp Drogon::Drogon spdlog::spdlog_header_only OpenSSL::SSL OpenSSL::Crypto)

file(COPY config.yaml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
