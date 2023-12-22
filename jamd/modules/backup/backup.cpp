#include "backup.h"

// 初始化全局变量
std::mutex log_mutex;
const String progress_file = "backup_progress.json";

// 读取进度文件
JsonValue load_progress() {
    std::ifstream file(progress_file, std::ifstream::binary);
    JsonValue progress;
    Json::CharReaderBuilder builder;
    String errs;

    if (file) {
        if (!Json::parseFromStream(builder, file, &progress, &errs)) {
            spdlog::error("无法解析进度文件: {}" + errs);
        }
    }
    return progress;
}

// 保存进度文件
void save_progress(const JsonValue &progress) {
    ofstream file(progress_file, ofstream::binary);
    if (file) {
        Json::StreamWriterBuilder writer;
        file << Json::writeString(writer, progress);
    }
}

// 计算文件的 SHA-256 哈希值
void calculate_sha256_hash(const Path &filename, HashBuffer &outputBuffer) {
    std::ifstream file(filename, ios::binary);
    if (!file) {
        LockGuard lock(log_mutex);
        spdlog::error("无法打开文件: {}" + filename.string());
        outputBuffer.assign(SHA256_DIGEST_LENGTH, 0);
        return;
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        LockGuard lock(log_mutex);
        spdlog::error("无法创建 EVP_MD_CTX.");
        return;
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        LockGuard lock(log_mutex);
        spdlog::error("初始化 SHA-256 失败.");
        EVP_MD_CTX_free(ctx);
        return;
    }

    std::vector<char> buffer(32768);
    while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
        if (EVP_DigestUpdate(ctx, buffer.data(), file.gcount()) != 1) {
            LockGuard lock(log_mutex);
            spdlog::error("SHA-256 更新失败.");
            EVP_MD_CTX_free(ctx);
            return;
        }
    }

    outputBuffer.resize(SHA256_DIGEST_LENGTH);
    unsigned int length;
    if (EVP_DigestFinal_ex(ctx, outputBuffer.data(), &length) != 1) {
        LockGuard lock(log_mutex);
        spdlog::error("完成 SHA-256 哈希失败.");
        outputBuffer.clear();
    }

    EVP_MD_CTX_free(ctx);
}

// 复制文件
void b_copy_file(const Path &src, const Path &dst) {
    std::ifstream srcStream(src, ios::binary);
    ofstream dstStream(dst, ios::binary | ios::trunc);

    if (!srcStream || !dstStream) {
        LockGuard lock(log_mutex);
        spdlog::error(std::format("打开文件错误: {} -> {}", src.string(), dst.string()));
        return;
    }

    dstStream << srcStream.rdbuf();
    fs::permissions(dst, fs::status(src).permissions());

    LockGuard lock(log_mutex);
    spdlog::info(std::format("已复制: {} 到 {}", src.string(), dst.string()));
}

// 判断文件是否已修改
bool is_file_modified(const Path &srcPath, const JsonValue &progress) {
    auto last_write_time = fs::last_write_time(srcPath);
    auto file_size = fs::file_size(srcPath);

    if (progress.isMember(srcPath.string())) {
        const auto &file_info = progress[srcPath.string()];
        return file_info["size"].asUInt64() != file_size ||
               file_info["last_write_time"].asUInt64() != last_write_time.time_since_epoch().count();
    }
    return true;  // 如果进度文件中没有记录该文件，则认为它已修改
}

// 更新进度文件
void update_progress(const Path &srcPath, const HashBuffer &hash, JsonValue &progress) {
    auto last_write_time = fs::last_write_time(srcPath);
    auto file_size = fs::file_size(srcPath);

    JsonValue file_info;
    file_info["hash"] = String(hash.begin(), hash.end());
    file_info["size"] = static_cast<Json::UInt64>(file_size);
    file_info["last_write_time"] = static_cast<Json::UInt64>(last_write_time.time_since_epoch().count());

    progress[srcPath.string()] = file_info;
}

// 处理单个文件
void process_file(const Path &srcPath, const Path &dstPath, JsonValue &progress) {
    HashBuffer srcHash;

    // 计算源文件的哈希值
    calculate_sha256_hash(srcPath, srcHash);

    // 如果目标文件不存在，或者文件已修改，则复制文件并更新进度
    if (!fs::exists(dstPath) || is_file_modified(srcPath, progress)) {
        b_copy_file(srcPath, dstPath);
        update_progress(srcPath, srcHash, progress);
    }
}

// 增量备份
void incremental_backup(const Path &sourceDir, const Path &destDir) {
    std::stack<Path> dirs;
    dirs.push(sourceDir);

    // 加载进度文件
    JsonValue progress = load_progress();

    std::vector<Future> futures;

    while (!dirs.empty()) {
        Path currentDir = dirs.top();
        dirs.pop();

        Path currentDestDir = destDir / currentDir.filename();
        if (!fs::exists(currentDestDir)) {
            fs::create_directory(currentDestDir);
        }

        for (const auto &entry: fs::directory_iterator(currentDir)) {
            if (fs::is_directory(entry.status())) {
                dirs.push(entry.path());
            } else if (fs::is_regular_file(entry.status())) {
                const Path &srcPath = entry.path();
                Path dstPath = currentDestDir / entry.path().filename();

                // 使用异步任务处理文件哈希计算和复制
                futures.push_back(std::async(std::launch::async, process_file, srcPath, dstPath, std::ref(progress)));
            }
        }
    }

    // 等待所有异步任务完成
    for (auto &fut: futures) {
        fut.get();
    }

    // 备份完成后保存进度
    save_progress(progress);
}
