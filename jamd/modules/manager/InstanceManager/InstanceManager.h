#pragma once

#include <vector>
#include <unordered_map>
#include "SingleServer.hpp"
#include "../../../utils/Singleton.h"
#include "../../../structs/config.h"
#include "../../ext/alias.h"

using Instance = SingleServer;

namespace jamd::managers
{
    /**
     * @brief 管理服务器实例的类
     */
    class InstanceManager final : public Singleton<InstanceManager>
    {
        friend class Singleton;
        Vector<ServerConfig> server_configs;
        UnorderedMap<String, String> config_path;

    public:
        /**
         * @brief
         * @param configObj 由配置文件添加服务器
         */
        void add(const ServerConfig& configObj);

        /**
         * @brief
         * @param name 要移除的服务器的名字
         */
        bool remove(const String& name);

        /**
         * @brief
         * @param id 要移除的服务器的id
         */
        bool remove(int id);

        /**
         * @brief
         * @param instances 要移除的服务器实例id的列表
         */
        void remove(const Vector<int>& instances);

        /**
         * @brief
         * @param id 要停止的服务器的id
         */
        bool stop(int id);

        /**
         * @brief
         * @param name 要停止的服务器的名字
         */
        bool stop(const String& name);

        /**
         * @brief
         * @param instance 要启动的服务器的引用
         */
        static int launch(::Instance& instance);

        /**
         * @brief
         * @param id 要启动的服务器的id
         */
        int launch(int id);

        /**
         * @brief
         * @param name 要启动的服务器的名称
         */
        int launch(const String& name);

        /**
         * @brief
         * @param instances 要启动的服务器的列表
         */
        bool launch(const Vector<int>& instances);

        /**
         * @brief
         * @param instance 要重启的服务器的引用
         */
        static int relaunch(::Instance& instance);

        /**
         * @brief
         * @param name 要启动的服务器的名称
         */
        int relaunch(const String& name);

        /**
         * @brief
         * @param id 要启动的服务器的id
         */
        int relaunch(int id);

        /**
         * @brief
         * @param instances 要启动的服务器的列表
         */
        bool relaunch(const Vector<int>& instances);

        /**
         * @brief
         * @param id 服务器的id
         */
        Vector<String> log(int id);

        /**
         * @brief
         * @param name 服务器的名称
         */
        Vector<String> log(const String& name);

        Optional<::Instance> getInstance(int id);

        Optional<::Instance> getInstance(const String& name);

    private:
        InstanceManager()
        {
            val configs = File("config").getFiles(".yaml");
            spdlog::info("Loading config(s)");
            for (auto config : configs)
            {
                val it = YAML::LoadFile(config).as<ServerConfig>();
                server_configs.push_back(it);
                config_path.insert({to_string(it.id), config.string()});
                spdlog::debug("Config #{} named {} from file {}", it.id, it.name, config.string());
            }
            if (val sz = configs.size(); sz > 1)
                spdlog::info("{} configs loaded", sz);
            else
                spdlog::info("Loaded one config");
        }
    };
}
