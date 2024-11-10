#pragma once

#include <vector>
#include <unordered_set>
#include "../../tmsmt/SingleServer.hpp"
#include "../../../utils/Singleton.h"

using String = std::string;
using Instance = SingleServer;
using InstanceObjs = std::vector<Instance>;
using InstanceParams = std::vector<String>;
using InstanceLogs = std::vector<String>;
using InstanceList = std::vector<int>;

namespace jamd::structs {
    struct ConfObj;
}

namespace jamd::managers {
    class InstanceManager final : public Singleton<InstanceManager> {
        friend class Singleton<InstanceManager>;

    public:
        void add(const ::Instance &instance);

        void add(const jamd::structs::ConfObj &configObj);

        void remove(const String &name);

        void remove(int id);

        void remove(const InstanceList &instances);

        void launch(::Instance instance);

        void launch(int id);

        void launch(const String &name);

        void launch(const InstanceList &instances);

        void relaunch(const ::Instance &instance);

        void relaunch(const String &name);

        void relaunch(int id);

        void relaunch(const InstanceList &instances);

        InstanceLogs log(int id);

        InstanceLogs log(const String &name);

        std::optional<const ::Instance> getInstance(int id);

        std::optional<const ::Instance> getInstance(const String &name);

    private:
        InstanceManager();
        InstanceObjs instances;
        InstanceList instanceIds;
    };
}
