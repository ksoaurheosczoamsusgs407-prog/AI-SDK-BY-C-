#include "../../include/util/myLog.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <functional>

namespace Log{
    std::shared_ptr<spdlog::logger> _logger = nullptr;
    std::mutex _mutex;

    Lagger::Lagger()
    {}

    void Lagger::InitLagger(const spdlog::level::level_enum &grade, const std::string &savepath, const std::string &format)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if(_logger == nullptr)
        {
            // 没有初始化过 智能指针是空的
            spdlog::flush_on(grade);
            spdlog::init_thread_pool()

        }
    }
}