cmake_minimum_required(VERSION 3.27)
project(jamd VERSION 0.1.0)

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
find_package(hiredis REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(Drogon CONFIG REQUIRED)
find_package(spdlog REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(unofficial-argon2 CONFIG REQUIRED)

add_executable(jamd main.cpp
        modules/backup/backup.cpp
        modules/backup/backup.h
        modules/manager/InstanceManager/InstanceManager.cpp
        modules/manager/InstanceManager/InstanceManager.h
        modules/manager/InstanceManager/SingleServer.hpp
        modules/manager/ConfigFileManager/ConfigFileManager.cpp
        modules/manager/ConfigFileManager/ConfigFileManager.h
        modules/grpc/LaunchService.cpp
        modules/grpc/LaunchService.h
        modules/grpc/GrpcServer.cpp
        modules/grpc/GrpcServer.h
        modules/parser/interfaces/ILogParser.h
        modules/parser/StdoutParser.h
        modules/preset/presets.h
        modules/parser/vanilla.h
        modules/PasswordHasher.h
        utils/Singleton.h

        controllers/Instances.h
        controllers/User.h
        controllers/Instances.cpp
        controllers/User.cpp
        controllers/System.h
        controllers/System.cpp

        models/Instances.cc
        models/Instances.h
        models/Users.cc
        models/Users.h

        plugins/autoAddHeaders/AutoAddHeaders.cpp
        plugins/autoAddHeaders/AutoAddHeaders.h
        plugins/log/RequestLogger.h
        plugins/log/RequestLogger.cpp
        plugins/checkFeatureEnabled/CheckFeatureEnabled.cpp
        plugins/checkFeatureEnabled/CheckFeatureEnabled.h

        ext/marcos/ktstyle.h
        ext/marcos/orm.h
        ext/marcos/drogon.h

        ext/extFuns.h
        ext/alias.h
        ext/jsonSerialization.h
        ext/yamlSerialization.h
        ext/styledORM.h

        structs/api/LaunchResult.h
        structs/api/RegisterInfo.h
        structs/api.h
        structs/config/ServerConfig.h
        structs/config/JamdConfig.h
        structs/config.h
        ${GENERATED_SOURCES}
)

target_include_directories(jamd PRIVATE modules/grpc)

if(MSVC)
    target_compile_options(jamd PRIVATE /experimental:module /D_WIN32_WINNT=0x0A00)
else()
    target_compile_options(jamd PRIVATE -fmodules-ts)
endif()

target_link_libraries(jamd PRIVATE
        Boost::filesystem
        Boost::process
        PostgreSQL::PostgreSQL
        hiredis
        yaml-cpp::yaml-cpp
        JsonCpp::JsonCpp
        Drogon::Drogon
        spdlog::spdlog_header_only
        OpenSSL::SSL
        OpenSSL::Crypto
        ${_GRPC_GRPCPP}
        gRPC::grpc++_reflection
        unofficial::argon2::libargon2
)

file(COPY drogon.yaml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY jamd.yaml DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(GLOB allConfigFiles "${CMAKE_CURRENT_SOURCE_DIR}/config/*")
file(COPY ${allConfigFiles} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/config")
