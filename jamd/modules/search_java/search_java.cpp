#include "search_java.h"

namespace fs = std::filesystem;
namespace bp = boost::process;
using Path = fs::path;

using Keywords = std::vector<std::string>;
using ExcludedDirs = std::vector<Path>;
using OptionalDepth = std::optional<int>;
using ResultList = std::vector<JavaResult>;
using Mutex = std::mutex;

JavaSearchThread::JavaSearchThread(Path path, ResultList &result_list, Mutex &lock, const Keywords &priority_keywords,
                                   bool strict_mode, const Keywords &excluded_keywords,
                                   std::unordered_set<Path> &excluded_dirs,
                                   const OptionalDepth max_depth, std::counting_semaphore<> &semaphore)
    : path_(std::move(path)), result_list_(result_list), lock_(lock),
      priority_keywords_(priority_keywords), strict_mode_(strict_mode), excluded_keywords_(excluded_keywords),
      excluded_dirs_(excluded_dirs), max_depth_(max_depth), semaphore_(semaphore) {
}

void JavaSearchThread::operator()() const {
    semaphore_.acquire(); // 获取信号量
    search_dirs(path_);
    semaphore_.release(); // 释放信号量
}

bool inline is_executable(const Path &path) {
#ifdef _WIN32
    return path.extension() == ".exe";
#else
    std::error_code ec;
    const auto status = fs::status(path, ec);
    if (ec) {
        spdlog::error("fs::status: {}", ec);
        return false;
    }

    if (is_regular_file(status)) {
        const auto perms = status.permissions();
        return (perms & fs::perms::owner_exec) != fs::perms::none ||
               (perms & fs::perms::group_exec) != fs::perms::none ||
               (perms & fs::perms::others_exec) != fs::perms::none;
    }
    return false;
#endif
}

void JavaSearchThread::search_dirs(const Path &path, int current_depth) const {
    if (max_depth_ && current_depth >= *max_depth_) {
        return; // 达到最大深度，停止搜索
    }

    try {
        std::queue<Path> directories;
        directories.push(path);

        while (!directories.empty()) {
            auto current_dir = directories.front();
            directories.pop();

            for (const auto &entry: fs::directory_iterator(current_dir)) {
                if (entry.is_directory()) {
                    const auto &dir_path = entry.path();
                    if (is_excluded(dir_path))
                        continue;

                    if (strict_mode_ && !has_priority_keyword(dir_path))
                        continue;

                    directories.push(dir_path);
                } else if (entry.is_regular_file()) {
                    auto file_path = entry.path();
                    if (auto [version, is_jdk] = check_java_version(file_path);
                        is_executable(file_path) && version.has_value()) {
                        std::lock_guard guard(lock_);
                        result_list_.emplace_back(JavaResult{
                            file_path, version.value(), is_jdk
                        });
                    }
                }
            }
        }
    } catch (const fs::filesystem_error &e) {
        spdlog::warn(e.what());
    }
}

[[nodiscard]] bool JavaSearchThread::is_excluded(const Path &dir) const {
    // 使用 unordered_set 提高排除目录检查效率
    if (excluded_dirs_.contains(dir)) {
        return true;
    }

    // 检查目录名是否包含排除关键词
    return std::ranges::any_of(excluded_keywords_, [&](const auto &keyword) {
        return dir.string().find(keyword) != std::string::npos;
    });
}

[[nodiscard]] bool JavaSearchThread::has_priority_keyword(const Path &dir) const {
    return std::ranges::any_of(priority_keywords_, [&](const auto &keyword) {
        return dir.string().find(keyword) != std::string::npos;
    });
}

auto JavaSearchThread::check_java_version(const Path &java_path) -> decltype(std::make_pair<std::nullopt, false>) {
    try {
        bp::ipstream pipe_stream;
        bp::child c(java_path.string() + " -version", bp::std_err > pipe_stream);

        std::string result;
        std::string line;
        while (pipe_stream && std::getline(pipe_stream, line) && !line.empty()) {
            result += line + "\n";
        }

        c.wait();

        // 改进的正则表达式，适应更多版本格式
        std::regex version_regex(R"((?:openjdk|oracle|java\s)(\d+)\.(\d+)\.(\d+)(?:_(\d+))?(-\S+)?)");
        if (std::smatch match; std::regex_search(result, match, version_regex)) {
            std::string version;
            for (size_t i = 1; i < match.size(); ++i) {
                if (match[i].matched) {
                    if (!version.empty()) {
                        version += ".";
                    }
                    version += match[i].str();
                }
            }

            // 判断是否是 JDK
            bool type = result.find("Java HotSpot") != std::string::npos;
            return std::make_pair(version, type);
        }
    } catch (const std::exception &e) {
        spdlog::error(e.what());
    }
    return std::make_pair<std::nullopt, false>;
}

std::vector<Path> get_search_paths() {
    std::vector<Path> paths;
#ifdef _WIN32
    for (char drive = 'A'; drive <= 'Z'; ++drive) {
        std::string drive_path = std::string(1, drive) + ":\\";
        if (fs::exists(drive_path)) {
            paths.emplace_back(drive_path);
        }
    }
#else
    paths.push_back("/usr");
    paths.push_back("/opt");
#endif
    return paths;
}

ResultList parallel_search_java(const std::vector<Path> &paths, const SearchConfig &config) {
    ResultList result_list;
    result_list.reserve(1024); // 预分配一定的空间，避免多次扩容
    Mutex lock;
    std::counting_semaphore semaphore(config.max_threads); // 创建信号量，限制最大线程数
    std::unordered_set excluded_dirs(config.excluded_dirs.begin(), config.excluded_dirs.end());
    std::vector<std::future<void> > futures;

    for (const auto &path: paths) {
        futures.emplace_back(
            std::async(std::launch::async, JavaSearchThread(path, result_list, lock, config.priority_keywords,
                                                            true, config.excluded_keywords,
                                                            excluded_dirs, config.max_depth, semaphore)));
    }

    for (auto &future: futures) {
        future.get();
    }

    return result_list;
}
