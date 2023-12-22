#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <boost/asio.hpp>
#include <boost/process.hpp>

enum class ServerType {
    JAVA,
    BINARY
};

struct SingleServer {
    ServerType type;
    std::string executor;
    int xms, xmx;
    std::string server_path;
    std::string server_file;
    std::vector<std::string> server_options;
    std::string config_name;

    std::unique_ptr<boost::process::child> server_process;
    std::unique_ptr<boost::process::opstream> stdin_stream;
    std::unique_ptr<boost::process::async_pipe> stdout_pipe, stderr_pipe;
    boost::asio::io_context io_context;

    explicit SingleServer(ServerType type,
                          std::string executor,
                          int xms,
                          int xmx,
                          std::string server_path,
                          std::string server_file,
                          std::vector<std::string> server_options);

    static std::vector<std::string> defaultJavaOptions();

    int start();

    void exit();

    void stop();

    void command(const std::string &cmd) const;

    void start_async_read(std::unique_ptr<boost::process::async_pipe> &pipe, bool is_stderr,
                          const std::function<void(const std::string &)> &callback);

private:
    void cleanup();
};
