cmake_minimum_required(VERSION 3.28)
project(tmsmt_core LANGUAGES CXX VERSION 0.0.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(tmsmt_core SHARED SingleServer.cpp SingleServer.hpp)
add_library(tmsmt_core_static STATIC SingleServer.cpp SingleServer.hpp)

set_target_properties(tmsmt_core PROPERTIES OUTPUT_NAME "tmsmt_core_dyn")
set_target_properties(tmsmt_core_static PROPERTIES OUTPUT_NAME "tmsmt_core")

find_package(Boost CONFIG REQUIRED COMPONENTS filesystem process)

include_directories(${Boost_INCLUDE_DIRS})

if(WIN32)
    target_link_libraries(tmsmt_core PRIVATE ws2_32)
    target_link_libraries(tmsmt_core_static PRIVATE ws2_32)
endif()
