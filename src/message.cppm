/********************************************************************************
* @Author : hexne
* @Date   : 2025/12/28 22:37:22
********************************************************************************/

module;
#include <httplib.h>
export module message;
import std;
import nlohmann.json;
import config;

export class Message {
    std::string user_number_;
    std::string message_;
    long long reply_id_;
public:
    Message(std::string user_number, std::string message, long long reply_id = 0)
        : user_number_(std::move(user_number)), message_(std::move(message)), reply_id_(reply_id) {  }

    [[nodiscard]] std::string to_json() const {
        nlohmann::json root;
        root["user_id"] = user_number_;

        nlohmann::json msg_array = nlohmann::json::array();

        // 如果有 reply_id，必须放在最前面
        if (reply_id_ != 0) {
            msg_array.push_back({
                {"type", "reply"},
                {"data", {{"id", reply_id_}}}
            });
        }

        // 文本消息
        msg_array.push_back({
            {"type", "text"},
            {"data", {{"text", message_}}}
        });

        root["message"] = msg_array;
        return root.dump(4);
    }


};

export bool send_message(const std::string& send_path,const Message& message) {
    httplib::Client cli(config.host, config.http.port);
    httplib::Headers headers = {
        {"Authorization", config.http.token}
    };

    auto res = cli.Post(send_path, headers, message.to_json(), "application/json");
    if (res)
        return true;
    std::cerr << "Request failed: " << httplib::to_string(res.error()) << std::endl;
    return false;
}
export bool send_private_message(const Message& message) {
    return send_message("/send_private_msg", message);
}

