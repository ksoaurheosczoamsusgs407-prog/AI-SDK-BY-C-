#include <iostream>
#include <spdlog/spdlog.h>
#include <sys/types.h>
#include <memory>
#include <string>

namespace Log{
    struct Lagger{
        private:
            void InitLagger(const spdlog::level::level_enum &grade, const std::string &savepath, const std::string &format);

        private:
            Lagger();

            Lagger(const Lagger&) = delete;

            Lagger &operator=(const Lagger &) = delete;

        private:
            static std::shared_ptr<spdlog::logger> _logger;
            static std::mutex _mutex;
    };
}// myLog end