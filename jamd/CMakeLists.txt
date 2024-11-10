cmake_minimum_required(VERSION 3.27)
project(jamd VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(Boost CONFIG REQUIRED COMPONENTS filesystem)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(JsonCpp CONFIG REQUIRED)
find_package(Drogon REQUIRED)
find_package(spdlog REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(OpenSSL REQUIRED)
add_executable(jamd main.cpp
        modules/backup/backup.cpp
        modules/backup/backup.h
        modules/search_java/search_java.cpp
        modules/search_java/search_java.h
        modules/manager/InstanceManager/InstanceManager.cpp
        modules/manager/InstanceManager/InstanceManager.h
        modules/manager/InstanceManager/ConfigObj.cpp
        utils/Singleton.h
        controllers/Instances.h
        controllers/User.h
        controllers/Instances.cpp
        controllers/User.cpp
        modules/manager/ConfigFileManager/ConfigFileManager.cpp
        modules/manager/ConfigFileManager/ConfigFileManager.h
)

if (MSVC)
    target_compile_options(jamd PRIVATE /experimental:module /D_WIN32_WINNT=0x0A00)
else ()
    target_compile_options(jamd PRIVATE -fmodules-ts)
endif ()

add_subdirectory("modules/tmsmt")

target_link_libraries(jamd PRIVATE Boost::filesystem PostgreSQL::PostgreSQL yaml-cpp::yaml-cpp  JsonCpp::JsonCpp Drogon::Drogon spdlog::spdlog_header_only tmsmt_core_static OpenSSL::SSL OpenSSL::Crypto)

file(COPY config.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
