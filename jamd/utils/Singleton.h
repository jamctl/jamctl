#pragma once

#include <iostream>
#include <memory>

namespace jamd {
    template<typename T>
    class Singleton {
    public:
        static T &Instance() {
            static T *instance = new T();
            return *instance;
        }

        // 禁止拷贝构造和赋值操作
        Singleton(const Singleton &) = delete;

        Singleton &operator=(const Singleton &) = delete;

    protected:
        Singleton() = default;

        virtual ~Singleton() = default;
    };
}

