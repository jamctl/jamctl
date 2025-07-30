#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <system_error>
#include <fstream>
#include <dirent.h>
#include <spdlog/spdlog.h>

#include "alias.h"

namespace fs = std::filesystem;


/**
 * @class File
 * @author MojaveHao
 * @brief java.io.File impl in C++
 */
class File
{
    Path path;

public:
    explicit File(const String& pathname) : path(pathname)
    {
    }

    File(const File& parent, const String& child) : path(parent.path / child)
    {
    }

    File(const String& parent, const String& child) : path(parent) { path /= child; }

    [[nodiscard]] String getName() const { return path.filename().string(); }
    [[nodiscard]] String getPath() const { return path.generic_string(); }
    [[nodiscard]] String getAbsolutePath() const { return fs::absolute(path).generic_string(); }

    [[nodiscard]] String getParent() const
    {
        const auto parent = path.parent_path();
        return parent.empty() ? "" : parent.generic_string();
    }

    // 状态检查
    [[nodiscard]] bool exists() const noexcept { return fs::exists(path); }
    [[nodiscard]] bool isFile() const noexcept { return fs::is_regular_file(path); }
    [[nodiscard]] bool isDirectory() const noexcept { return fs::is_directory(path); }
    [[nodiscard]] bool isHidden() const { return path.filename().string().front() == '.'; }

    [[nodiscard]] std::filesystem::perms canRead() const
    {
        std::error_code ec;
        return fs::status(path, ec).permissions() & fs::perms::owner_read;
    }

    [[nodiscard]] std::filesystem::perms canWrite() const
    {
        std::error_code ec;
        return fs::status(path, ec).permissions() & fs::perms::owner_write;
    }

    // 文件操作
    [[nodiscard]] uintmax_t length() const
    {
        std::error_code ec;
        const auto size = fs::file_size(path, ec);
        return ec ? 0 : size;
    }

    [[nodiscard]] bool createNewFile() const
    {
        if (exists()) return false;
        std::ofstream file(path);
        return file.is_open();
    }

    [[nodiscard]] bool mkdir() const
    {
        std::error_code ec;
        return fs::create_directory(path, ec) && !ec;
    }

    [[nodiscard]] bool mkdirs() const
    {
        std::error_code ec;
        fs::create_directories(path, ec);
        return !ec && exists();
    }

    [[nodiscard]] bool remove() const
    {
        std::error_code ec;
        fs::remove(path, ec);
        return !ec && !exists();
    }

    [[nodiscard]] bool renameTo(const File& dest) const
    {
        std::error_code ec;
        fs::rename(path, dest.path, ec);
        return !ec && !exists() && dest.exists();
    }

    // 目录操作
    [[nodiscard]] Vector<String> list() const
    {
        Vector<String> names;
        if (!isDirectory()) return names;

        for (const auto& entry : fs::directory_iterator(path))
        {
            names.push_back(entry.path().filename().generic_string());
        }
        return names;
    }

    [[nodiscard]] Vector<File> listFiles() const
    {
        Vector<File> files;
        if (!isDirectory()) return files;

        for (const auto& entry : fs::directory_iterator(path))
        {
            files.emplace_back(entry.path().generic_string());
        }
        return files;
    }

    Vector<Path> getFiles(
        const String& extension = "",
        const bool ignore_case = true,
        const int max_depth = 0
    ) const
    {
        // 检查目录是否存在
        if (!this->exists())
            throw std::invalid_argument("Directory does not exist: " + this->path.string());
        if (!this->isDirectory())
            throw std::invalid_argument("Path is not a directory: " + this->path.string());

        Vector<Path> result;
        String ext_lower = extension;

        // 忽略大小写,如果可能
        if (ignore_case && !extension.empty())
        {
            std::ranges::transform(ext_lower, ext_lower.begin(),
                                   [](const unsigned char c) { return std::tolower(c); });
        }

        // 处理无限深度情况
        const bool unlimited_depth = max_depth == -1;

        // 递归遍历函数
        Function<void(const Path&, int)> traverse;
        traverse = [&](const Path& current_path, const int current_depth)
        {
            // 检查深度限制
            if (!unlimited_depth && current_depth > max_depth)
                return;

            try
            {
                for (const auto& entry : fs::directory_iterator(current_path))
                {
                    try
                    {
                        // 如果是目录则递归遍历
                        if (entry.is_directory())
                            traverse(entry.path(), current_depth + 1);
                            // 如果是文件则检查后缀
                        else if (entry.is_regular_file())
                        {
                            if (extension.empty())
                                result.push_back(entry.path());
                            else
                            {
                                String file_ext = entry.path().extension().string();

                                // 处理大小写
                                if (ignore_case)
                                    std::ranges::transform(file_ext, file_ext.begin(),
                                                           [](unsigned char c) { return std::tolower(c); });

                                // 检查后缀匹配
                                if (file_ext == ext_lower)
                                    result.push_back(entry.path());
                            }
                        }
                    }
                    catch (const fs::filesystem_error& e)
                    {
                        // 跳过无权限访问的文件/目录
                        spdlog::warn(e.what());
                    }
                }
            }
            catch (const fs::filesystem_error& e)
            {
                // 处理目录访问错误
                spdlog::error(e.what());
            }
        };

        traverse(this->path, 0);
        return result;
    }


    // 时间戳方法
    [[nodiscard]] uint64_t lastModified() const
    {
        const auto time = fs::last_write_time(path);
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            time.time_since_epoch()).count();
    }
};
