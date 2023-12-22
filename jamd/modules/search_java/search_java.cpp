#include "search_java.h"

JavaSearchThread::JavaSearchThread(Path path, ResultList &result_list, Mutex &lock, Keywords priority_keywords,
                                   bool strict_mode, Keywords excluded_keywords, ExcludedDirs excluded_dirs,
                                   OptionalDepth max_depth)
        : path_(std::move(path)), result_list_(result_list), lock_(lock), priority_keywords_(std::move(priority_keywords)),
          strict_mode_(strict_mode), excluded_keywords_(std::move(excluded_keywords)), excluded_dirs_(std::move(excluded_dirs)),
          max_depth_(max_depth) {}

void JavaSearchThread::operator()() {
    search_dirs(path_);
}

void JavaSearchThread::search_dirs(const Path &path) {
    try {
        std::queue<Path> directories;
        directories.push(path);

        while (!directories.empty()) {
            auto current_dir = directories.front();
            directories.pop();

            for (const auto &entry : fs::directory_iterator(current_dir)) {
                if (max_depth_ && 0 >= *max_depth_) {
                    break;
                }

                if (entry.is_directory()) {
                    const auto &dir_path = entry.path();
                    if (is_excluded(dir_path)) {
                        continue;
                    }

                    if (strict_mode_ && !has_priority_keyword(dir_path)) {
                        continue;
                    }

                    directories.push(dir_path);  // Use BFS to add subdirectories
                } else if (entry.is_regular_file()) {
                    auto file_path = entry.path();
                    if (file_path.extension() == ".java") {
                        auto version = check_java_version(file_path);
                        if (version) {
                            std::lock_guard<Mutex> guard(lock_);
                            result_list_.emplace_back(file_path, *version);
                        }
                    }
                }
            }
        }
    } catch (const fs::filesystem_error &e) {
        spdlog::warn(path.string());
    }
}

[[nodiscard]] bool JavaSearchThread::is_excluded(const Path &dir) const {
    // Check if the directory is in the excluded_dirs_ list
    if (std::ranges::any_of(excluded_dirs_, [&](const auto &excluded_dir) {
        return fs::equivalent(dir, excluded_dir);
    })) {
        return true;
    }

    // Check if any of the excluded keywords are found in the directory path
    if (std::ranges::any_of(excluded_keywords_, [&](const auto &keyword) {
        return dir.string().find(keyword) != std::string::npos;
    })) {
        return true;
    }

    return false;
}

[[nodiscard]] bool JavaSearchThread::has_priority_keyword(const Path &dir) const {
    return std::ranges::any_of(priority_keywords_, [&](const auto &keyword) {
        return dir.string().find(keyword) != std::string::npos;
    });
}

std::optional<std::string> JavaSearchThread::check_java_version(const Path &java_path) {
    try {
        // Use Boost.Process to run the command and capture the output
        bp::ipstream pipe_stream;
        bp::child c(java_path.string() + " -version", bp::std_err > pipe_stream);

        std::string result;
        std::string line;
        while (pipe_stream && std::getline(pipe_stream, line) && !line.empty()) {
            result += line + "\n";
        }

        c.wait();

        // 使用正则提取版本号
        std::regex version_regex(R"((\d+)(?:\.(\d+))?(?:\.(\d+))?(?:[._](\d+))?(?:-(.+))?)");
        std::smatch match;
        if (std::regex_search(result, match, version_regex)) {
            std::string version;
            for (size_t i = 1; i < match.size(); ++i) {
                if (match[i].matched) {
                    if (!version.empty()) {
                        version += ".";
                    }
                    version += match[i].str();
                }
            }
            return version;
        }
    } catch (const std::exception &e) {
        spdlog::error(e.what());
    }
    return std::nullopt;
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

ResultList parallel_search_java(const std::vector<Path> &paths, const Keywords &priorities, bool strict,
                                const Keywords &excluded_keywords, const ExcludedDirs &excluded_dirs,
                                OptionalDepth max_depth) {
    ResultList result_list;
    Mutex lock;
    ThreadPool futures;

    for (const auto &path : paths) {
        futures.emplace_back(
                std::async(std::launch::async, JavaSearchThread(path, result_list, lock, priorities, strict,
                                                                excluded_keywords, excluded_dirs, max_depth)));
    }

    for (auto &future : futures) {
        future.get();
    }

    return result_list;
}
