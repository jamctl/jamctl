#pragma once

#include <filesystem>
#include <future>
#include <mutex>
#include <optional>
#include <queue>
#include <regex>
#include <semaphore> // C++20 的信号量
#include <string>
#include <unordered_set>
#include <vector>
#include <boost/process.hpp>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;
namespace bp = boost::process;

using Path = fs::path;

// Java 查找结果
struct JavaResult {
    Path file_path;
    std::string version;
    bool is_jdk;
};

// 搜索相关的配置
struct SearchConfig {
    std::vector<std::string> priority_keywords;
    std::vector<std::string> excluded_keywords;
    std::vector<Path> excluded_dirs;
    std::optional<int> max_depth;
    int max_threads;
};

// JavaSearchThread 类声明
class JavaSearchThread {
public:
    JavaSearchThread(Path path,
                     std::vector<JavaResult> &result_list,
                     std::mutex &lock,
                     const std::vector<std::string> &priority_keywords,
                     bool strict_mode,
                     const std::vector<std::string> &excluded_keywords,
                     std::unordered_set<Path> &excluded_dirs,
                     std::optional<int> max_depth,
                     std::counting_semaphore<> &semaphore);

    // 运算符重载，作为线程执行函数
    void operator()() const;

private:
    void search_dirs(const Path &path, int current_depth = 0) const;

    bool is_excluded(const Path &dir) const;

    bool has_priority_keyword(const Path &dir) const;

    static std::pair<std::optional<std::string>, bool> check_java_version(const Path &java_path);

    Path path_;
    std::vector<JavaResult> &result_list_;
    std::mutex &lock_;
    const std::vector<std::string> &priority_keywords_;
    bool strict_mode_;
    const std::vector<std::string> &excluded_keywords_;
    std::unordered_set<Path> &excluded_dirs_;
    std::optional<int> max_depth_;
    std::counting_semaphore<> &semaphore_;
};

// 辅助函数：检查文件是否可执行
bool is_executable(const Path &path);

// 获取搜索路径，Windows 和类 Unix 系统的不同处理
std::vector<Path> get_search_paths();

// 多线程并行搜索 Java 安装路径
std::vector<JavaResult> parallel_search_java(const std::vector<Path> &paths, const SearchConfig &config);
