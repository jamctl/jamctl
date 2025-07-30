#include "InstanceManager.h"

#include "../ConfigFileManager/ConfigFileManager.h"

namespace jamd::managers
{
    void InstanceManager::add(const ServerConfig& configObj)
    {
        const Path p = ConfigFileManager::Instance().daemonConfig.instances.save_path;
        std::ofstream configFile(p/(configObj.name+".yaml"));
        var e = YAML::Emitter();
        e << configObj;
        configFile << e.c_str();
        configFile.close();
    }

    bool InstanceManager::remove(const String& name)
    {
        if (val r = std::ranges::find_if(server_configs, [=](const ServerConfig& config)
        {
            return config.name == name;
        }); r != server_configs.end())
        {
            remove(r->id);
            return true;
        }
        return false;
    }

    bool InstanceManager::remove(const int id)
    {
        if (val r = config_path.find(to_string(id)); r != config_path.end())
        {
            std::remove(r->second.c_str());
            config_path.erase(r);
            return true;
        }
        return false;
    }

    void InstanceManager::remove(const Vector<int>& instances)
    {
        for (val instance : instances)
        {
            remove(instance);
        }
    }

    bool InstanceManager::stop(const int id)
    {
        var r = getInstance(id);
        if (!r.has_value())
            return false;
        r.value().stop();
        return true;
    }

    bool InstanceManager::stop(const String& name)
    {
        var r = getInstance(name);
        if (!r.has_value())
            return false;
        r.value().stop();
        return true;
    }

    int InstanceManager::launch(const String& name)
    {
        if (var r = getInstance(name); r.has_value())
            return r.value().start();
        return -1;
    }

    int InstanceManager::launch(const int id)
    {
        if (var r = getInstance(id); r.has_value())
            return r.value().start();
        return -1;
    }

    int InstanceManager::launch(::Instance& instance)
    {
        return instance.start();
    }

    bool InstanceManager::launch(const Vector<int>& instances)
    {
        for (val instance : instances)
            if (launch(instance) == 0)
                return false;
        return true;
    }

    int InstanceManager::relaunch(::Instance& instance)
    {
        instance.stop();
        return launch(instance);
    }

    int InstanceManager::relaunch(const String& name)
    {
        if (var r = getInstance(name); r.has_value())
        {
            r.value().stop();
            return r.value().start();
        }
        return -1;
    }

    int InstanceManager::relaunch(const int id)
    {
        if (var r = getInstance(id); r.has_value())
        {
            r.value().stop();
            return r.value().start();
        }
        return -1;
    }

    bool InstanceManager::relaunch(const Vector<int>& instances)
    {
        for (val instance : instances)
            if (launch(instance) == 0)
                return false;
        return true;
    }

    Vector<String> InstanceManager::log(const int id)
    {
        if (var r = getInstance(id); r.has_value())
            return r.value().log();
        return {""};
    }

    Vector<String> InstanceManager::log(const String& name)
    {
        if (var r = getInstance(name); r.has_value())
            return r.value().log();
        return {""};
    }

    Optional<Instance> InstanceManager::getInstance(const int id)
    {
        val r = std::ranges::find_if(server_configs, [=](const ServerConfig& config)
        {
            return config.id == id;
        });
        if (r == server_configs.end()) return std::nullopt;
        return r->instance();
    }

    Optional<Instance> InstanceManager::getInstance(const String& name)
    {
        val r = std::ranges::find_if(server_configs, [&](const ServerConfig& config)
        {
            return config.name == name;
        });
        if (r == server_configs.end()) return std::nullopt;
        return r->instance();
    }
}
