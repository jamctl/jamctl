#pragma once

#include <vector>
#include <unordered_set>
#include "SingleServer.hpp"
#include "../../../utils/Singleton.h"
#include "../../../structs/config.h"
#include "../../ext/alias.h"

using Instance = SingleServer;
using InstanceObjs = Vector<Instance>;
using InstanceLogs = Vector<String>;
using InstanceList = Vector<int>;

namespace jamd::managers
{
    /**
     * @brief 管理服务器实例的类
     */
    class InstanceManager final : public Singleton<InstanceManager>
    {
        friend class Singleton;

    public:

        /**
         * @brief
         * @param instance 现存实例引用
         */
        void add(const ::Instance& instance);

        /**
         * @brief
         * @param configObj 由配置文件添加服务器
         */
        void add(const structs::config::ServerConfig& configObj);

        /**
         * @brief
         * @param name 要移除的服务器的名字
         */
        void remove(const String& name);

        /**
         * @brief
         * @param id 要移除的服务器的id
         */
        void remove(int id);

        /**
         * @brief
         * @param instances 要移除的服务器实例id的列表
         */
        void remove(const InstanceList& instances);

        /**
         * @brief
         * @param instance 要启动的服务器的引用
         */
        int launch(const ::Instance& instance);

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
        int launch(const InstanceList& instances);

        /**
         * @brief
         * @param instance 要重启的服务器的引用
         */
        int relaunch(const ::Instance& instance);

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
        int relaunch(const InstanceList& instances);

        InstanceLogs log(int id);

        InstanceLogs log(const String& name);

        Optional<RefWrapper<::Instance>> getInstance(int id);

        Optional<RefWrapper<::Instance>> getInstance(const String& name);

    private:
        InstanceManager();

        InstanceObjs instances;
        InstanceList instanceIds;
    };
}
