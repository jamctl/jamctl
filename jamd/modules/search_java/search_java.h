#pragma once

#include <string>
#include <utility>
#include <vector>
#include <filesystem>
#include <regex>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include <optional>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <boost/process.hpp>
#include <queue>
#include <utility>

namespace fs = std::filesystem;
namespace bp = boost::process;

using Path = fs::path;
using ResultList = std::vector<std::pair<Path, std::string>>;
using Mutex = std::mutex;
using Keywords = std::vector<std::string>;
using ExcludedDirs = std::vector<Path>;
using OptionalDepth = std::optional<int>;
using ThreadPool = std::vector<std::future<void>>;

class JavaSearchThread {
public:
    // 构造函数
    JavaSearchThread(Path path, ResultList &result_list, Mutex &lock, Keywords priority_keywords = {},
                     bool strict_mode = false, Keywords excluded_keywords = {},
                     ExcludedDirs excluded_dirs = {}, OptionalDepth max_depth = std::nullopt);

    void operator()();

private:
    Path path_;
    ResultList &result_list_;
    Mutex &lock_;
    Keywords priority_keywords_;
    bool strict_mode_;
    Keywords excluded_keywords_;
    ExcludedDirs excluded_dirs_;
    OptionalDepth max_depth_;

    void search_dirs(const Path &path);

    [[nodiscard]] bool is_excluded(const Path &dir) const;

    [[nodiscard]] bool has_priority_keyword(const Path &dir) const;

    static std::optional<std::string> check_java_version(const Path &java_path);
};

std::vector<Path> get_search_paths();

ResultList parallel_search_java(const std::vector<Path> &paths, const Keywords &priorities, bool strict,
                                const Keywords &excluded_keywords, const ExcludedDirs &excluded_dirs,
                                OptionalDepth max_depth);