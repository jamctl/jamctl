#pragma once

#include <filesystem>
#include <utility>

#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

#include "../../../ext/alias.h"
#include "../../../ext/extFuns.h"
#include "../../../ext/marcos/ktstyle.h"
#include "../../parser/interfaces/ILogParser.h"

namespace asio = boost::asio;
namespace bp = boost::process::v2;
namespace fs = std::filesystem;
namespace bf = boost::filesystem;
using std::exception;
using std::to_string, std::unique_ptr, std::make_unique, std::make_shared;

/**
 * @class SingleServer
 * @brief 管理单个服务器进程的生命周期（支持Java服务器）
 *
 * 封装针对单个服务器实例的管理功能
 */
class SingleServer {
    bool type_is_java;
    int xms, xmx;
    String executor;
    String server_path;
    String server_file;
    Vector<StringView> server_options;

    IOContext CTX;
    WritablePipe input { CTX };
    ReadablePipe output { CTX };
    ReadablePipe error { CTX };

    UniquePtr<bp::process> process;
    Thread io_thread;
    bool done;
    MutableList<String> command_line;
    MutableList<UniquePtr<LogParserInterface>> log_parsers;

    public:
        /**
     * @brief 构造函数
     * @param type_is_java 是否为Java服务器（默认true）
     * @param executor 执行器路径及参数（默认"java"）
     * @param xms 初始堆内存（GB，默认1）
     * @param xmx 最大堆内存（GB，默认4）
     * @param work_dir 服务器工作目录路径
     * @param jar_file 服务器JAR文件名（如server.jar,仅在type_is_java为true时有实际作用）
     * @param server_options 附加启动参数
     */
        explicit SingleServer(
            const bool type_is_java = true,
            String executor = "java",
            const int xms = 1,
            const int xmx = 4,
            String work_dir = "",
            String jar_file = "server.jar",
            Vector<StringView> server_options = {}
        ) : type_is_java(type_is_java),
            xms(xms),
            xmx(xmx),
            executor(std::move(executor)),
            server_path(std::move(work_dir)),
            server_file(std::move(jar_file)),
            server_options(std::move(server_options)),
            done(false)
        {}

        ~SingleServer()
        {
            exit(); // 确保对象销毁时自动清理
            if (process) {
                process->wait();
            }
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
            if (type_is_java) {
                args.emplace_back("-Xms" + to_string(xms) + "G");
                args.emplace_back("-Xmx" + to_string(xmx) + "G");
                args.emplace_back("-jar");
            }
            args.emplace_back(server_file_path.string());
            args.insert(args.end(), server_options.begin(), server_options.end());

            try {
                process = make_unique<bp::process>(
                    bp::process(CTX,
                                executor,
                                args,
                                bp::process_start_dir(server_path),
                                bp::process_stdio { input, output, output }));
                start_async_read(output, false);
                start_async_read(error, true);
                io_thread = std::thread([this]
                {
                    CTX.run();
                });
            } catch (const exception& e) {
                spdlog::error("Failed to start instance: {}", e.what());
                return 1;
            }
            return 0;
        }

        /**
     * @brief 强制终止服务器进程
     */
        void exit()
        {
            cleanup();
        }

        /**
     * @brief 优雅停止服务器（发送stop命令）
     */
        void stop()
        {
            command("stop");
            cleanup();
        }

        /**
     * @brief 向服务器发送控制台命令
     * @param cmd 要发送的命令（不需要换行符）
     */
        void command(const String& cmd)
        {
            if (input.is_open()) {
                if (!done) {
                    command_line.emplace_back(cmd);
                    spdlog::warn("Server not launched, command lined, current line: \n{}", expand(command_line));
                    return;
                }
                // 使用异步写入标准输入
                String data = cmd + "\n";
                asio::async_write(input, asio::buffer(data),
                                  [](const boost::system::error_code& ec, size_t)
                                  {
                                      if (ec)
                                          spdlog::error("Error sending command: ", ec.message());
                                  }
                );
            }
        }

        /**
     * @brief 获取服务器输出
     */
        Vector<String> log()
        {
            Vector<char> data;
            boost::system::error_code ec;
            asio::read(output, asio::dynamic_buffer(data), ec);
            if (ec && ec != asio::error::eof) {
                return { "" };
            }
            const String str_data(data.begin(), data.end());
            return split(str_data, '\n');
        }

        /**
     * @brief 注册日志解析器
     * @param parser 解析器的UniquePtr
     */
        void register_parser(UniquePtr<LogParserInterface> parser)
        {
            log_parsers.emplace_back(std::move(parser));
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
                [this, buffer, &pipe, is_stderr](const boost::system::error_code& ec, const size_t bytes_transferred)
                {
                    if (ec)
                        return;
                    const String current_output(buffer->data(), bytes_transferred);
                    if (!done && current_output.contains("Done")) {
                        done = true;
                        for (auto cmd : command_line)
                            command(cmd);
                    }
                    for (auto& parser : log_parsers) {
                        if (!is_stderr)
                            for (auto str : split(current_output, '\n'))
                                parser.get()->parse(str);
                        else
                            for (auto str : split(current_output, '\n'))
                                parser.get()->parse_err(str);
                    }
                    this->start_async_read(pipe, is_stderr);
                }
            );
        }

        /**
     * @brief 清理进程资源
     */
        void cleanup()
        {
            if (process) {
                process->terminate();
                process.reset();
            }

            // 安全关闭管道
            if (input.is_open())
                input.close();
            if (output.is_open())
                output.close();
            if (error.is_open())
                error.close();

            // 停止并等待IO线程
            CTX.stop();
            if (io_thread.joinable()) {
                io_thread.join();
            }
        }
};