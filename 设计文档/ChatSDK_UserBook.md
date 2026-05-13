# ai_chat_sdk 使用手册

## 核心类与结构体

### 1. 配置类（Config 及其子类）

所有配置类均定义在 `ai_chat_sdk` 命名空间下，需使用 `std::shared_ptr` 管理。

**基类：`Config`**  
子类：`APIConfig`（云端模型）、`OllamaConfig`（本地 Ollama）

| 类型 | 字段 | 说明 |
|------|------|------|
| `APIConfig` | `_modelName` | 模型名称，如 `"deepseek-chat"`, `"gpt-4o-mini"`, `"gemini-2.0-flash"` |
| | `_apiKey` | API 密钥 |
| | `_temperature` | 温度系数 0~1，越高越灵活但可能增加幻觉 |
| | `_maxTokens` | 单次回复最大 token 数 |
| `OllamaConfig` | `_modelName` | Ollama 模型名（如 `"deepseek-r1:1.5b"`） |
| | `_modelDesc` | 模型描述 |
| | `_endpoint` | Ollama 服务端点（如 `"http://localhost:11434"`） |
| | `_temperature` | 同上 |
| | `_maxTokens` | 同上 |

#### 2. 消息与会话结构
- **`Message`** – 单条消息  
  - `_messageId` : 消息唯一 ID  
  - `_role` : 角色（`"user"` / `"assistant"`）  
  - `_content` : 内容  
  - `_timestamp` : 时间戳（Unix 秒）

- **`Session`** – 一个对话会话  
  - `_sessionId` : 会话 ID  
  - `_modelName` : 使用的模型名  
  - `_createdAt` : 创建时间戳  
  - `_updatedAt` : 最后更新时间戳  
  - `_messages` : `std::vector<Message>`

- **`ModelInfo`** – 可用模型信息  
  - `_modelName` : 模型名  
  - `_modelDesc` : 模型描述

#### 3. 核心类：`ChatSDK`
- **初始化日志**（可选）  
  ```cpp
  bite::Logger::initLogger("appName", "stdout", spdlog::level::info);
  ```
- **创建 SDK 实例**  
  ```cpp
  ai_chat_sdk::ChatSDK chat_sdk;
  ```

## 主要方法

| 方法 | 说明 |
|------|------|
| `bool initModels(std::vector<std::shared_ptr<Config>> configs)` | 初始化模型，验证连通性，建立连接池。返回 `true` 成功 |
| `std::string createSession(const std::string& modelName)` | 创建新会话，返回会话 ID（空串表示失败） |
| `std::string sendMessage(const std::string& sessionId, const std::string& message)` | 发送消息并全量返回助手回复（同步阻塞） |
| `void sendMessageStream(const std::string& sessionId, const std::string& message, Callback callback)` | 流式发送消息，`callback` 形式：`void(const std::string& chunk, bool done)`，`done==true` 表示流结束 |
| `std::shared_ptr<Session> getSession(const std::string& sessionId)` | 获取会话对象，包含全部历史消息 |
| `std::vector<std::string> getSessionLists()` | 返回所有会话 ID 列表 |
| `bool deleteSession(const std::string& sessionId)` | 删除会话，`true` 成功 |
| `std::vector<ModelInfo> getAvailableModels()` | 返回当前已初始化的模型列表 |

## HTTP API（ChatServer 提供的接口）

### 路由表

| 方法 | 路径 | 用途 |
|------|------|------|
| `POST` | `/api/session` | 创建会话 |
| `GET` | `/api/sessions` | 获取所有会话列表 |
| `GET` | `/api/models` | 获取可用模型列表 |
| `DELETE` | `/api/session/{sessionId}` | 删除会话 |
| `GET` | `/api/session/{sessionId}/history` | 获取会话历史消息 |
| `POST` | `/api/message` | 发送消息（全量返回） |
| `POST` | `/api/message/async` | 发送消息（SSE 流式返回） |

### 通用响应格式
```json
{
  "success": true/false,
  "message": "描述信息",
  "data": { ... }  // 具体数据
}
```

### 示例
- **创建会话**  
  请求体：`{"model": "deepseek-chat"}`  
  响应 `data`：`{"session_id": "...", "model": "..."}`

- **发送全量消息**  
  请求体：`{"session_id": "...", "message": "你好"}`  
  响应 `data`：包含 `assistant_message` 等

- **流式消息**  
  请求体同上，响应头 `Content-Type: text/event-stream`  
  数据格式：`data: "chunk内容"\n\n`，结束发送 `data: [DONE]\n\n`

## 使用示例

### 1. 直接使用 SDK（不依赖 HTTP 服务）
```cpp
#include <ai_chat_sdk/ChatSDK.h>
#include <ai_chat_sdk/util/myLog.h>

int main() {
    bite::Logger::initLogger("demo", "stdout", spdlog::level::info);
    ai_chat_sdk::ChatSDK sdk;

    auto deepseekCfg = std::make_shared<ai_chat_sdk::APIConfig>();
    deepseekCfg->_modelName = "deepseek-chat";
    deepseekCfg->_apiKey = std::getenv("DEEPSEEK_API_KEY");
    deepseekCfg->_temperature = 0.7;
    deepseekCfg->_maxTokens = 2048;

    std::vector<std::shared_ptr<ai_chat_sdk::Config>> configs = {deepseekCfg};
    if (!sdk.initModels(configs)) {
        std::cerr << "Init failed" << std::endl;
        return -1;
    }

    std::string sessionId = sdk.createSession("deepseek-chat");
    if (sessionId.empty()) return -1;

    // 同步消息
    std::string reply = sdk.sendMessage(sessionId, "你好");
    std::cout << "AI: " << reply << std::endl;

    // 流式消息
    sdk.sendMessageStream(sessionId, "讲个笑话", [](const std::string& chunk, bool done) {
        std::cout << chunk;
        if (done) std::cout << std::endl << "[结束]" << std::endl;
    });

    sdk.deleteSession(sessionId);
    return 0;
}
```

### 2. 部署并调用 HTTP 服务
使用 `ChatServer` 启动服务后，前端可直接通过 HTTP 请求交互，静态页面放在 `www` 目录下（首页默认 `index.html`）。

```cpp
#include "ChatServer.h"
// 初始化 ServerConfig 并启动
ai_chat_server::ServerConfig config;
config.deepseekAPIKey = "...";
config.port = 8080;
config.host = "0.0.0.0";
// ... 其他配置
ai_chat_server::ChatServer server(config);
server.start();
// 阻塞保持运行或设计退出逻辑
```

之后从浏览器访问 `http://<ip>:8080` 即可。