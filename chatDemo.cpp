#include <ai_chat_sdk/ChatSDK.h>
#include <ai_chat_sdk/util/myLog.h>
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <string>

void sendMassageStream(ai_chat_sdk::ChatSDK &chat_sdk, std::string &sessionId)
{
    std::cout << "-----发送消息-----" << std::endl;
    std::cout << "用户输入 > ";
    std::string message;
    std::getline(std::cin, message);
    std::cout << "-----发送完成-----" << std::endl;
    chat_sdk.sendMessageStream(sessionId, message, [](const std::string &response, bool done)
                               {
        std::cout << response << std::endl;
        if(done){
            std::cout << "-----------接收完成----------" << std::endl;
        } });
}

int main()
{
    bite::Logger::initLogger("aiChatServer", "stdout", spdlog::level::info);

    ai_chat_sdk::ChatSDK chat_sdk;

    ai_chat_sdk::APIConfig deepseekConfig;

    // 这里传输的环境变量，没有配置目前
    deepseekConfig._apiKey = std::getenv("deepseek_apikey");
    // 温度系数：0~1 越大越容易出现幻觉 ，但回答会更灵活
    deepseekConfig._temperature = 0.7;
    // 单次回复的最长长度 1.5~2个tocken是一个汉字 1~1.3个tocken是一个英文单词
    deepseekConfig._maxTokens = 2048;
    deepseekConfig._modelName = "deepseek-chat";

    // 定义数组是为了管理所有的模型统一进行初始化
    // 智能指针类型是Config而不是APIConfig，是因为还有本地OllamaConfig，父类是Config，统一管理
    std::vector<std::shared_ptr<ai_chat_sdk::Config>> configs;
    configs.push_back(std::make_shared<ai_chat_sdk::Config>(deepseekConfig));

    // 创建API客户端，设置http连接池，验证密钥到端点的联通性，并不是直接全加载更像是握手验证占用计算资源较少
    chat_sdk.initModels(configs);

    std::cout << "--------------------创建模型--------------------" << std::endl;
    // 为保证后续消息的上下文连续，分离对话
    // 后续同一个模型的多个对话也是通过这个进行
    std::string sessionId = chat_sdk.createSession(deepseekConfig._modelName);

    int userstat = 1;
    while (1)
    {
        std::cout << "-------1. send message 0. exit-----------------" << std::endl;
        std::cin >> userstat;
        if (userstat == 0)
            break;
        getchar();
        sendMassageStream(chat_sdk, sessionId);
    }

    std::cout << "----------对话结束断开连接----------" << std::endl;

    return 0;
}