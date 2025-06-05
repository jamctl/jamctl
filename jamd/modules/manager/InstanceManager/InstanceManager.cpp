#include "InstanceManager.h"

namespace jamd::managers
{
    InstanceManager::InstanceManager() = default;

    void InstanceManager::add(const ::Instance& instance)
    {
    }

    void InstanceManager::add(const structs::config::ServerConfig& configObj)
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
    }

    int InstanceManager::launch(int id)
    {
    }

    int InstanceManager::launch(const ::Instance& instance)
    {
    }

    int InstanceManager::launch(const InstanceList& instances)
    {
    }

    int InstanceManager::relaunch(const ::Instance& instance)
    {
    }

    int InstanceManager::relaunch(const String& name)
    {
    }

    int InstanceManager::relaunch(int id)
    {
    }

    int InstanceManager::relaunch(const InstanceList& instances)
    {
    }

    InstanceLogs InstanceManager::log(int id)
    {
        return {};
    }

    InstanceLogs InstanceManager::log(const String& name)
    {
        return {};
    }

    std::optional<std::reference_wrapper<Instance>> InstanceManager::getInstance(int id)
    {
        return {};
    }

    std::optional<std::reference_wrapper<Instance>> InstanceManager::getInstance(const String& name)
    {
        return {};
    }
}
