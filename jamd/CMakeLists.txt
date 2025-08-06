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
if (CMAKE_CROSSCOMPILING)
    find_program(_PROTOBUF_PROTOC protoc)
else ()
    get_target_property(PROTOC_LOCATION protobuf::protoc LOCATION)
    set(_PROTOBUF_PROTOC "${PROTOC_LOCATION}")
endif ()
find_package(gRPC CONFIG REQUIRED)
message(STATUS "gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
if (CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else ()
    get_target_property(GRPC_PLUGIN_LOCATION gRPC::grpc_cpp_plugin LOCATION)
    set(_GRPC_CPP_PLUGIN_EXECUTABLE "${GRPC_PLUGIN_LOCATION}")
endif ()
message(STATUS "gRPC C++ Plugin at: ${_GRPC_CPP_PLUGIN_EXECUTABLE}")

file(GLOB PROTO_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc/proto/*.proto")

set(GENERATED_SOURCES)
set(GENERATED_HEADERS)
foreach (proto ${PROTO_FILES})
    get_filename_component(proto_name ${proto} NAME_WE)
    set(src "${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc/${proto_name}.pb.cc")
    set(hdr "${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc/${proto_name}.pb.h")
    set(grpc_src "${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc/${proto_name}.grpc.pb.cc")
    set(grpc_hdr "${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc/${proto_name}.grpc.pb.h")
    execute_process(
            COMMAND ${_PROTOBUF_PROTOC} --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc
            --grpc_out=${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc
            --plugin=protoc-gen-grpc=${_GRPC_CPP_PLUGIN_EXECUTABLE}
            -I ${CMAKE_CURRENT_SOURCE_DIR}/modules/grpc/proto
            ${proto}
    )
    list(APPEND GENERATED_SOURCES ${src} ${grpc_src})
    list(APPEND GENERATED_HEADERS ${hdr} ${grpc_hdr})
endforeach ()

find_package(Boost CONFIG REQUIRED COMPONENTS filesystem process locale)
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
        ext/file.h

        structs/api/LaunchResult.h
        structs/api/RegisterInfo.h
        structs/api.h
        structs/config/ServerConfig.h
        structs/config/JamdConfig.h
        structs/config.h
        ${GENERATED_SOURCES}
        modules/grpc/JamdManageService.cpp
        modules/grpc/JamdManageService.h
)

target_include_directories(jamd PRIVATE modules/grpc)

if (MSVC)
    target_compile_options(jamd PRIVATE /experimental:module /D_WIN32_WINNT=0x0A00)
else ()
    target_compile_options(jamd PRIVATE -fmodules-ts)
endif ()

target_link_libraries(jamd PRIVATE
        Boost::filesystem
        Boost::process
        Boost::locale
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

# i18n with Boost.Locale
find_program(XGETTEXT xgettext)
find_program(MSGMERGE msgmerge)
find_program(MSGINIT msginit)
find_program(MSGFMT msgfmt)

# 检查是否找到所有必要的工具
if (NOT XGETTEXT OR NOT MSGMERGE OR NOT MSGINIT OR NOT MSGFMT)
    message(WARNING "The translation tool is incomplete, and translation-related targets will be unavailable.")
    return()
else ()
    message(STATUS "XGETTEXT: ${XGETTEXT}")
    message(STATUS "MSGMERGE: ${MSGMERGE}")
    message(STATUS "MSGINIT: ${MSGINIT}")
    message(STATUS "MSGFMT: ${MSGFMT}")
endif ()

set(TRANSLATION_DOMAIN "jamd")
set(PO_DIR "${CMAKE_SOURCE_DIR}/modules/internationalization/po")
set(POT_FILE "${PO_DIR}/${TRANSLATION_DOMAIN}.pot")
set(SRC_DIRS ${CMAKE_SOURCE_DIR})

file(MAKE_DIRECTORY ${PO_DIR})

file(GLOB_RECURSE SRC_FILES
        "${SRC_DIRS}/*.cpp"
        "${SRC_DIRS}/*.h"
        "${SRC_DIRS}/*.cc"
)
file(GLOB_RECURSE REMOVE_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/*.*")
list(REMOVE_ITEM SRC_FILES ${REMOVE_SOURCES})

add_custom_target(
        extract_messages
        COMMAND ${XGETTEXT}
        --keyword=_               # Boost.Locale常用的翻译宏
        --keyword=N_              # 用于标记需要翻译但不立即翻译的字符串
        --output=${POT_FILE}
        --package-name=${PROJECT_NAME}
        --package-version=${PROJECT_VERSION}
        --from-code=utf-8         # 源代码编码
        --copyright-holder="${PROJECT_AUTHOR}"
        ${SRC_FILES}
        COMMENT "Extracting strings from source code to ${POT_FILE}"
)
file(GLOB PO_FILES "${PO_DIR}/*.po")

foreach (PO_FILE ${PO_FILES})
    get_filename_component(LANG ${PO_FILE} NAME_WE)
    add_custom_target(
            update_${LANG}_messages
            COMMAND ${MSGMERGE} --update --backup=none ${PO_FILE} ${POT_FILE}
            DEPENDS extract_messages
            COMMENT "Updating ${LANG} messages"
    )
endforeach ()

add_custom_target(
        update_all_messages
        DEPENDS ${PO_FILES}
        COMMENT "Updating all messages"
)

# 编译所有.po文件为二进制.mo文件
foreach (PO_FILE ${PO_FILES})
    get_filename_component(LANG ${PO_FILE} NAME_WE)
    set(MO_FILE "${CMAKE_BINARY_DIR}/locale/${LANG}/LC_MESSAGES/${TRANSLATION_DOMAIN}.mo")

    # 确保输出目录存在
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/locale/${LANG}/LC_MESSAGES")

    add_custom_target(
            compile_${LANG}_messages ALL
            COMMAND ${MSGFMT} -o ${MO_FILE} ${PO_FILE}
            DEPENDS ${PO_FILE}
            COMMENT "Compiling ${LANG} to ${MO_FILE}"
    )

    # 安装.mo文件到系统标准位置
    install(
            FILES ${MO_FILE}
            DESTINATION share/locale/${LANG}/LC_MESSAGES
            COMPONENT translations
    )
endforeach ()

# 函数: 创建新的语言翻译
function(add_translation LANG)
    set(PO_FILE "${PO_DIR}/${LANG}.po")
    add_custom_target(
            create_${LANG}_translation
            COMMAND ${MSGINIT} --input=${POT_FILE} --output=${PO_FILE} --locale=${LANG}
            DEPENDS extract_messages
            COMMENT "Generating message for ${LANG}"
    )
endfunction()

add_translation(zh_CN)
