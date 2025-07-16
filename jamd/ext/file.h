#pragma once

#include <filesystem>
#include <vector>
#include <string>
#include <system_error>
#include <fstream>

#include "alias.h"

namespace fs = std::filesystem;


/**
 * @class File
 * @author MojaveHao
 * @brief java.io.File impl in C++
 */
class File
{
    fs::path path;

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

    // 时间戳方法
    [[nodiscard]] uint64_t lastModified() const
    {
        const auto time = fs::last_write_time(path);
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            time.time_since_epoch()).count();
    }
};
