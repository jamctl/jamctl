#include "InstanceManager.h"

namespace jamd::managers
{
    InstanceManager::InstanceManager() = default;

    void InstanceManager::add(const ::Instance& instance)
    {
    }

    void InstanceManager::add(const ServerConfig& configObj)
    {
    }

    void InstanceManager::remove(const String& name)
    {
    }

    void InstanceManager::remove(int id)
    {
    }

    void InstanceManager::remove(const InstanceList& instances)
    {
    }

    int InstanceManager::launch(const String& name)
    {
        return {};
    }

    int InstanceManager::launch(int id)
    {
    }

    int InstanceManager::launch(const ::Instance& instance)
    {
    }

    int InstanceManager::launch(const InstanceList& instances)
    {
        return {};
    }

    int InstanceManager::relaunch(const ::Instance& instance)
    {
        return {};
    }

    int InstanceManager::relaunch(const String& name)
    {
        return {};
    }

    int InstanceManager::relaunch(int id)
    {
        return {};
    }

    int InstanceManager::relaunch(const InstanceList& instances)
    {
        return {};
    }

    InstanceLogs InstanceManager::log(int id)
    {
        return {};
    }

    InstanceLogs InstanceManager::log(const String& name)
    {
        return {};
    }

    std::optional<RefWrapper<Instance>> InstanceManager::getInstance(int id)
    {
        return {};
    }

    std::optional<RefWrapper<Instance>> InstanceManager::getInstance(const String& name)
    {
        return {};
    }
}
