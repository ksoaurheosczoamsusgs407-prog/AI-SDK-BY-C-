#include <ai_chat_sdk/ChatSDK.h>
#include <ai_chat_sdk/util/myLog.h>
#include <iostream>
#include <vector>
#include <memory>
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
        if(done){
            std::cout << "-----------接收完成----------" << std::endl;
        } });
}

int main()
{
    bite::Logger::initLogger("aiChatServer", "stdout", spdlog::level::info);

    ai_chat_sdk::ChatSDK chat_sdk;

    ai_chat_sdk::APIConfig deepseekConfig;

    deepseekConfig._apiKey = std::getenv("deepseek_apikey"); // 这里传输的环境变量，但是我们没有获取对应AI的环境变量所以会报错
    deepseekConfig._temperature = 0.7;
    deepseekConfig._maxTokens = 2048;
    deepseekConfig._modelName = "deepseek-chat";

    std::vector<std::shared_ptr<ai_chat_sdk::Config>> configs;
    configs.push_back(std::make_shared<ai_chat_sdk::Config>(deepseekConfig));

    chat_sdk.initModels(configs);

    std::cout << "--------------------创建模型--------------------" << std::endl;
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