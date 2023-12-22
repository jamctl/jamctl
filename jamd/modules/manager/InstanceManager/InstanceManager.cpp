#include "InstanceManager.h"

namespace jamd::managers {
    InstanceManager::InstanceManager() = default;

    void InstanceManager::add(SingleServer instance) {
    }

    void InstanceManager::add(const jamd::structs::ConfObj &configObj) {
    }

    void InstanceManager::remove(const String &name) {
    }

    void InstanceManager::remove(int id) {
    }

    void InstanceManager::remove(const Instances &instances) {
    }

    void InstanceManager::launch(const String &name) {
    }

    void InstanceManager::launch(int id) {
    }

    void InstanceManager::launch(::Instance instance) {
    }

    void InstanceManager::launch(const Instances &instances) {
    }

    void InstanceManager::relaunch(const ::Instance &instance) {
    }

    void InstanceManager::relaunch(const String &name) {
    }

    void InstanceManager::relaunch(int id) {
    }

    void InstanceManager::relaunch(const Instances &instances) {
    }

    InstanceLogs InstanceManager::log(int id) {
        return {};
    }

    InstanceLogs InstanceManager::log(const String &name) {
        return {};
    }
}
