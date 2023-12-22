#include "SingleServer.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <utility>

#include <boost/asio.hpp>
#include <boost/process.hpp>

namespace asio = boost::asio;
namespace bp = boost::process;
namespace fs = std::filesystem;
using std::string, std::vector, std::exception;
using std::endl, std::cerr, std::cout, std::cin;
using std::to_string, std::unique_ptr, std::make_unique, std::make_shared, std::shared_ptr, std::function;

SingleServer::SingleServer(ServerType type,
                           string executor,
                           int xms,
                           int xmx,
                           string server_path,
                           string server_file,
                           vector<string> server_options)
        : type(type),
          executor(std::move(executor)),
          xms(xms),
          xmx(xmx),
          server_path(std::move(server_path)),
          server_file(std::move(server_file)),
          server_options(std::move(server_options)) {
    if (this->server_options.empty()) {
        this->server_options = defaultJavaOptions();
    }
}

vector<string> SingleServer::defaultJavaOptions() {
    return {
            "-XX:+UnlockExperimentalVMOptions",
            "-XX:MaxGCPauseMillis=100",
            "-XX:+DisableExplicitGC",
            "-XX:TargetSurvivorRatio=90",
            "-XX:G1NewSizePercent=50",
            "-XX:G1MaxNewSizePercent=80",
            "-XX:G1MixedGCLiveThresholdPercent=35",
            "-XX:+AlwaysPreTouch",
            "-XX:+ParallelRefProcEnabled",
            "-Dusing.aikars.flags=mcflags.emc.gs"
    };
}

int SingleServer::start() {
    if (xms > xmx) {
        cerr << "Error: Xms (" << xms << "G) cannot be greater than Xmx (" << xmx << "G)." << endl;
        return 2;
    }

    // 确定服务器文件路径
    fs::path server_file_path = fs::path(server_path) / server_file;
    if (server_file.find(".jar") == string::npos && type == ServerType::JAVA) {
        server_file_path += ".jar";
    }

    // 初始化流和管道
    try {
        stdin_stream = make_unique<bp::opstream>();
        stdout_pipe = make_unique<bp::async_pipe>(io_context);
        stderr_pipe = make_unique<bp::async_pipe>(io_context);
    } catch (const exception& e) {
        cerr << "Failed to initialize pipes: " << e.what() << endl;
        return 1;
    }

    // 构建启动参数
    vector<string> args{executor};
    args.insert(args.end(), server_options.begin(), server_options.end());

    if (type == ServerType::JAVA) {
        args.emplace_back("-Xms" + to_string(xms) + "G");
        args.emplace_back("-Xmx" + to_string(xmx) + "G");
        args.emplace_back("-jar");
    }
    args.emplace_back(server_file_path.string());

    // 启动服务器进程
    try {
        server_process = make_unique<bp::child>(
                args,
                bp::std_in < *stdin_stream,
                bp::std_out > *stdout_pipe,
                bp::std_err > *stderr_pipe,
                bp::start_dir = server_path
        );

        // 异步读取标准输出和标准错误
        start_async_read(stdout_pipe, false, [](const string &output) { cout << output; });
        start_async_read(stderr_pipe, true, [](const string &output) { cerr << output; });

    } catch (const exception &e) {
        cerr << "Failed to start server: " << e.what() << endl;
        return 1;
    }

    // 运行异步 I/O 上下文
    io_context.run();

    return 0;
}

void SingleServer::exit() {
    if (server_process && server_process->running()) {
        server_process->terminate();
        server_process->wait();
        cleanup();
    }
}

void SingleServer::stop() {
    command("stop");
    if (server_process) {
        server_process->wait();
        cleanup();
    }
}

void SingleServer::command(const string &cmd) const {
    if (stdin_stream) {
        *stdin_stream << cmd << endl;
        stdin_stream->flush();
    }
}

void SingleServer::start_async_read(unique_ptr<bp::async_pipe> &pipe, bool is_stderr,
                                    const function<void(const string &)> &callback) {
    auto buffer = make_shared<vector<char>>(1024);
    pipe->async_read_some(asio::buffer(*buffer),
                          [this, buffer, &pipe, is_stderr, callback](const boost::system::error_code &ec,
                                                                     size_t bytes_transferred) {
                              if (!ec) {
                                  string output(buffer->data(), bytes_transferred);
                                  callback(output);
                                  this->start_async_read(pipe, is_stderr, callback);
                              } else if (ec != asio::error::eof) {
                                  cerr << (is_stderr ? "Stderr" : "Stdout")
                                       << " read error: " << ec.message() << endl;
                              }
                          });
}

void SingleServer::cleanup() {
    server_process.reset();
    stdin_stream.reset();
    stdout_pipe.reset();
    stderr_pipe.reset();
}