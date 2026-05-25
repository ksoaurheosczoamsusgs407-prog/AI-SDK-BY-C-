#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <sys/types.h>


// 这里向直接用右值，但是想了一下，传参之后的数据仍然要用，并不一定就是右值，反而左值转右值开销不如直接引用传参
namespace ai_chat_sdk
{

    // 消息结构类
    struct Message
    {
        std::string _messageId; // 消息ID
        std::string _role;      // 发送消息的角色 存疑？
        std::string _content;   // 消息内容
        std::string _timestamp; // 发送的时间

        // 构造
        Message(const std::string &role = "", const std::string &content = "")
            : _role(role),
              _content(content),
              _timestamp(0)
        {
        }
    };

    struct Config
    {
        std::string _modelName;    // 模型名称
        double _temperature = 0.7; // 模型水温
        int maxtoken = 2048;       // 模型生成最大限制

        virtual ~Config() = default; // 让派生类调用自己的析构
    };

    // 通过API连接云端模型
    struct APIconfig : public Config
    {
        std::string api_key;
    };

    // Ollama连接本地模型
    struct Ollamam : public Config
    {
        // TO DO
    };

    struct ModeInfo
    {
        std::string _modelName;    // 模型名称
        std::string _modelDesc;    // 模型描述
        std::string _provider;     // 模型提供者
        std::string _endpoint;     // 模型API 固定路径
        bool _isAvailable = false; // 模型状态

        ModeInfo(const std::string &modelName = "", const std::string &modelDesc = "", const std::string &provider = "", const std::string &endpoint = "")
            : _modelName(modelName),
              _modelDesc(modelDesc),
              _provider(provider),
              _endpoint(endpoint)
        {
        }
    };

    struct Session
    {
        std::string _sessionId;         // 会话ID
        std::string _modleName;         // 会话使用的模型
        std::vector<Message> _messages; // 会话中的消息列表
        std::time_t _createdAt;         // 会话创建时间戳
        std::time_t _updatedAt;         // 会话最后更新时间戳

        Session(const std::string &modelName = "")
        {
        }
    };

} // end ai_chat_sdk