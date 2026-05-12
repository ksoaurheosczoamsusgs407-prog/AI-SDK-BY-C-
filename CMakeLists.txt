# 项目名称
project(AIChatDemo)

# 设置 C++17 标准，必须支持否则报错
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Debug 构建模式（方便调试）
set(CMAKE_BUILD_TYPE Debug)

# 添加可执行文件：从 cmdChatDemo.cpp 编译出 AIChatDemo
add_executable(AIChatDemo chatDemo.cpp)

# 查找 OpenSSL 依赖（必须安装）
find_package(OpenSSL REQUIRED)

# 头文件路径（自动从 OpenSSL 获取）
include_directories(${OPENSSL_INCLUDE_DIR})

# 库搜索路径（你的 ai_chat_sdk 所在目录）
link_directories(/usr/local/lib)

# 链接所有依赖库
target_link_libraries(AIChatDemo PRIVATE
    ai_chat_sdk
    fmt
    jsoncpp
    OpenSSL::SSL
    OpenSSL::Crypto
    gflags
    spdlog
    sqlite3
)