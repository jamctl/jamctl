cmake_minimum_required(VERSION 3.27)
project(jamd VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(Boost CONFIG REQUIRED COMPONENTS filesystem)
find_package(Drogon REQUIRED)
find_package(spdlog REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
add_executable(jamd main.cpp
        modules/backup/backup.cpp
        modules/backup/backup.h
        modules/search_java/search_java.cpp
        modules/search_java/search_java.h)

if (MSVC)
    target_compile_options(jamd PRIVATE /experimental:module)
else ()
    target_compile_options(jamd PRIVATE -fmodules-ts)
endif ()

add_subdirectory("modules/tmsmt")

target_link_libraries(jamd PRIVATE Boost::filesystem PostgreSQL::PostgreSQL Drogon::Drogon spdlog::spdlog_header_only yaml-cpp::yaml-cpp tmsmt_core_static OpenSSL::SSL OpenSSL::Crypto)
