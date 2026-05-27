#include <iostream>
// 核心头文件，包含日治其，级别，全局设置
#include <spdlog/spdlog.h>
#include <sys/types.h>
#include <memory>
#include <string>

namespace Log
{
    // 宏： #define 名字(参数列表) 实际替换内容
    // 对应：TRAC就是这个宏的名字，format就是一个占位符，并不是已存在的变量，...表示可变参数，就是除了format之外，调用者还能够传输 >= 0个其他参数
    // 可变参数在替换体中用__VA_ARGS__来进行访问， ##是防止__VA_ARGS__为空从而多出来一个都好导致语法错误
    // __FILE__	当前源文件名字符串，如 "main.cpp"
    // __LINE__ 当前行号的十进制整数 整型常量
    // __FUNCTION__ 或 __func__ 当前函数名字符串
    // std::string("[{:>10s}:{:<4d}]") C风格的占位符统一格式
#define TRACE(format, ...) Log::Logger::getLogger()->trace(std::string("[{:>10s}:{:<4d}]") + format, __FILE__, __LINE__, ##__VA_ARGS__)

    struct Lagger
    {
    private:
        // 日志等级 ， 日志存储路径， 日志名称
        static void InitLagger(const spdlog::level::level_enum &logLevel = spdlog::level::info, const std::string &logFile, const std::string &logName);
        static std::shared_ptr<spdlog::logger> getLogger();

    private:
        Lagger();
        Lagger(const Lagger &) = delete;
        Lagger &operator=(const Lagger &) = delete;

    private:
        static std::shared_ptr<spdlog::logger> _logger;
        static std::mutex _mutex;
    };
} // myLog end