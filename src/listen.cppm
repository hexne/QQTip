module;
#include <ixwebsocket/IXWebSocket.h>
export module listen;
import std;
import nlohmann.json;
import config;
import message;
import time;
import tip_task_manager;

LocalTime start_time = LocalTime::now();

void command_info(long long reply_id) {

    auto now = LocalTime::now() - start_time;
    auto time = std::format("QQTip持续运行 {}d, {:2}h {:2}m {:2}s",
            now.count<std::chrono::days>(), now.get<std::chrono::hours>(), now.get<std::chrono::minutes>(), now.get<std::chrono::seconds>());

    auto message = Message(config.admin, time, reply_id);
    send_private_message(message);
}

void command_tasks(long long reply_id) {
    tip_task_manager;

}

export void listen() {
    static std::map<std::string, std::function<void(long long)>> command_funcs = {
        { "/info", command_info },
        {"/tasks", command_tasks },
    };
    static ix::WebSocket ws;

    ws.setUrl(std::format("ws://{}:{}", config.host, config.ws.port));

    ws.setExtraHeaders({
        {"Authorization", config.ws.token}
    });

    ws.setOnMessageCallback([](const ix::WebSocketMessagePtr& msg) {
        if (msg->type != ix::WebSocketMessageType::Message)
            return;
        auto json = nlohmann::json::parse(msg->str);
        if (json["post_type"] != "message")
            return;
        auto message_str = json["message"];
        auto func = command_funcs.find(message_str);

        // 先只处理命令
        if (func == command_funcs.end())
            return;

        std::string user_number = std::to_string(static_cast<long long>(json["user_id"]));
        // 判断是否是管理员
        if (config.admin != user_number)
            return;

        long long reply_id = json["message_id"];
        func->second(reply_id);

    });

    ws.start();
}
