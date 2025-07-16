#pragma once

#include <iostream>
#include <filesystem>
#include <utility>

#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

#include "../../../ext/alias.h"
#include "../../../ext/marcos/ktstyle.h"

namespace asio = boost::asio;
namespace bp = boost::process::v2;
namespace fs = std::filesystem;
namespace bf = boost::filesystem;
using std::move, std::exception;
using std::endl, std::cerr, std::cout, std::cin;
using std::to_string, std::unique_ptr, std::make_unique, std::make_shared;

/**
 * @class SingleServer
 * @brief 管理单个服务器进程的生命周期（支持Java服务器）
 *
 * 封装针对单个服务器实例的管理功能
 */
class SingleServer
{
    bool type_is_java;
    int xms, xmx;
    String executor;
    String server_path;
    String server_file;
    Vector<StringView> server_options;

    IOContext CTX;
    WritablePipe input{CTX};
    ReadablePipe output{CTX};
    ReadablePipe error{CTX};

    UniquePtr<bp::process> process;

public:
    /**
     * @brief 构造函数
     * @param type_is_java 是否为Java服务器（默认true）
     * @param executor 执行器路径（默认"java"）
     * @param xms 初始堆内存（GB，默认1）
     * @param xmx 最大堆内存（GB，默认4）
     * @param server_path 服务器工作目录路径
     * @param server_file 服务器文件名（如server.jar）
     * @param server_options 附加启动参数
     */
    explicit SingleServer(const bool type_is_java = true,
                          String executor = "java",
                          const int xms = 1,
                          const int xmx = 4,
                          String server_path = "",
                          String server_file = "server.jar",
                          Vector<StringView> server_options = {})
        : type_is_java(type_is_java),
          xms(xms),
          xmx(xmx),
          executor(move(executor)),
          server_path(move(server_path)),
          server_file(move(server_file)),
          server_options(move(server_options))
    {
    }

    ~SingleServer()
    {
        exit(); // 确保对象销毁时自动清理
    }

    /**
     * @brief 启动服务器进程
     * @return 状态码：0成功，1启动失败，2内存参数错误
    */
    int start()
    {
        if (xms > xmx)
            return 2;

        Path server_file_path = Path(server_path) / server_file;
        if (server_file.find(".jar") == String::npos && type_is_java)
            server_file_path += ".jar";

        Vector<String> args;
        args.emplace_back("-Xms" + to_string(xms) + "G");
        args.emplace_back("-Xmx" + to_string(xmx) + "G");
        args.insert(args.end(), server_options.begin(), server_options.end());
        if (type_is_java)
            args.emplace_back("-jar");
        args.emplace_back(server_file_path.string());

        try
        {
            process = make_unique<bp::process>(
                bp::process(CTX,
                            fs::path(server_file_path),
                            args,
                            bp::process_stdio{input, output, output}));
        }
        catch (const exception& e)
        {
            spdlog::error("Failed to start instance: {}", e.what());
            return 1;
        }

        CTX.run();
        return 0;
    }

    /**
     * @brief 强制终止服务器进程
     */
    void exit()
    {
        if (process)
        {
            process->terminate();
            process->wait();
            cleanup();
        }
    }

    /**
     * @brief 优雅停止服务器（发送stop命令）
     */
    void stop()
    {
        command("stop");
        if (process)
        {
            process->wait();
            cleanup();
        }
    }

    /**
     * @brief 向服务器发送控制台命令
     * @param cmd 要发送的命令（不需要换行符）
     */
    void command(const String& cmd)
    {
        if (input.is_open())
        {
            // 使用异步写入标准输入
            String data = cmd + "\n";
            asio::async_write(input, asio::buffer(data),
                              [](const boost::system::error_code& ec, size_t)
                              {
                                  if (ec)
                                  {
                                      cerr << "Error sending command: " << ec.message() << endl;
                                  }
                              }
            );
        }
    }

private:
    /**
     * @brief 异步读取进程输出
     * @param pipe 要读取的管道（output/error）
     * @param is_stderr 是否是错误流
     */
    void start_async_read(asio::readable_pipe& pipe, bool is_stderr)
    {
        var buffer = make_shared<Vector<char>>(1024);
        pipe.async_read_some(
            asio::buffer(*buffer),
            [this, buffer, &pipe, is_stderr]
        (const boost::system::error_code& ec, const size_t bytes_transferred)
            {
                if (ec)
                    return;
                const String output(buffer->data(), bytes_transferred);
                if (is_stderr)
                    cerr << output;
                else
                    cout << output;
                this->start_async_read(pipe, is_stderr);
            }
        );
    }

    /**
     * @brief 清理进程资源
     */
    void cleanup()
    {
        process.reset();
        input.close();
        output.close();
        error.close();
    }
};
