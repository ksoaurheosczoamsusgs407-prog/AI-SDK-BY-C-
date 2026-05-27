#include "../../include/util/myLog.h"


// 基础文件输出目标
#include <spdlog/sinks/basic_file_sink.h>
// 带颜色的控制台输出目标
#include <spdlog/sinks/stdout_color_sinks.h>
// 异步日志库
#include <spdlog/async.h>
#include <functional>

namespace Log{
    std::shared_ptr<spdlog::logger> Log::_logger = nullptr;
    std::mutex Log::_mutex;

    Lagger::Lagger()
    {}

    void Lagger::InitLagger(const spdlog::level::level_enum &logLevel, const std::string &logFile, const std::string &logName)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if(_logger == nullptr)
        {
            // 设置全局自动刷新级别，当日志等级 >= logLevel 时候，日志会被立即刷新到文件
            // 从高至低：trace, debug, info, warn, error, critical, off
            spdlog::flush_on(logLevel);
            // 启用异步日志，将日志信息存放在队列中，由后台线程进行写入，队列大小后台进程数量，1个一般就够，太多会导致磁盘资源的竞争
            spdlog::init_thread_pool(32768, 1);
            if("stdcout" == logFile)
            {
                // 创建一个输出到控制台带颜色的日志器
                _logger = spdlog::stdout_color_st(logName);
            }
            else
            {
                // 创建一个文件输出的日志器，日志回呗写入到指定文件中
                _logger = spdlog::basic_logger_mt<spdlog::async_logger>(logName, logFile);
            }

            // 设置日志格式
            // [%H:%M:%S]:时分秒
            // [%n]：日志名称
            // [%-7l]：日志等级  %v：日志内容
            _logger->set_pattern("[%H:%M:%S][%n][%-7l]:%v");
            _logger->set_level(logLevel);
        }
    }

    std::shared_ptr<spdlog::logger> Log::getLogger()
    {
        return _logger;
    }
}// myLog.cpp end