#pragma once

#include <vector>
#include "../../tmsmt/SingleServer.hpp"
#include "../../../utils/Singleton.h"

using String = std::string;
using Instance = SingleServer;
using InstanceParams = std::vector<String>;
using InstanceLogs = std::vector<String>;
using Instances = std::vector<int>;

namespace jamd::structs {
    struct ConfObj;
}

namespace jamd::managers {
    class InstanceManager final : public Singleton<InstanceManager> {
        friend class Singleton<InstanceManager>;

    public:
        void add(::Instance instance);

        void add(const jamd::structs::ConfObj &configObj);

        void remove(const String &name);

        void remove(int id);

        void remove(const Instances &instances);

        void launch(::Instance instance);

        void launch(int id);

        void launch(const String &name);

        void launch(const Instances &instances);

        void relaunch(const ::Instance &instance);

        void relaunch(const String &name);

        void relaunch(int id);

        void relaunch(const Instances &instances);

        static InstanceLogs log(int id);

        static InstanceLogs log(const String &name);

    private:
        InstanceManager();
    };
}
