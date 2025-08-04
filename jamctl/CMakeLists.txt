cmake_minimum_required(VERSION 3.27)
project(jamctl VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_definitions(-DUSE_REFLECTION)

set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "protobuf ${Protobuf_VERSION}")
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
    find_program(_PROTOBUF_PROTOC protoc)
else()
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()
find_package(gRPC CONFIG REQUIRED)
message(STATUS "gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()
message(STATUS "gRPC C++ Plugin at: ${_GRPC_CPP_PLUGIN_EXECUTABLE}")

file(GLOB PROTO_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc/proto/*.proto")

set(GENERATED_SOURCES)
set(GENERATED_HEADERS)
foreach(proto ${PROTO_FILES})
    get_filename_component(proto_name ${proto} NAME_WE)
    set(src "${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc/${proto_name}.pb.cc")
    set(hdr "${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc/${proto_name}.pb.h")
    set(grpc_src "${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc/${proto_name}.grpc.pb.cc")
    set(grpc_hdr "${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc/${proto_name}.grpc.pb.h")
    add_custom_command(
            OUTPUT ${src} ${hdr} ${grpc_src} ${grpc_hdr}
            COMMAND ${_PROTOBUF_PROTOC}
            ARGS --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc
            --grpc_out=${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc
            --plugin=protoc-gen-grpc=${_GRPC_CPP_PLUGIN_EXECUTABLE}
            -I ${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc/proto
            ${proto}
            DEPENDS ${proto}
    )
    list(APPEND GENERATED_SOURCES ${src} ${grpc_src})
    list(APPEND GENERATED_HEADERS ${hdr} ${grpc_hdr})
endforeach()

find_package(Boost CONFIG REQUIRED COMPONENTS filesystem process)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(spdlog REQUIRED)

file(GLOB_RECURSE cpp_srcs "*.cpp")
file(GLOB_RECURSE cc_srcs "*.cc")
file(GLOB_RECURSE headers "*.h")

add_executable(jamctl main.cpp ${cpp_srcs} ${cc_srcs} ${headers})

target_include_directories(jamctl PRIVATE modules/grpc)

if(MSVC)
    target_compile_options(jamctl PRIVATE /experimental:module /D_WIN32_WINNT=0x0A00)
else()
    target_compile_options(jamctl PRIVATE -fmodules-ts)
endif()

target_link_libraries(jamctl PRIVATE
        Boost::filesystem
        Boost::process
        yaml-cpp::yaml-cpp
        JsonCpp::JsonCpp
        spdlog::spdlog_header_only
        ${GENERATED_SOURCES}
        gRPC::grpc++_reflection
)
