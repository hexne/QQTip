module;
#include <json/json.h>
export module message;
import std;

export class Message {
public:
    virtual ~Message() = default;
    virtual std::string to_json_string() = 0;
};

export class TextMessage : public Message {
    std::string send_to_user_number_;
    std::string message_;

public:
    TextMessage(std::string number, std::string message)
            : send_to_user_number_(std::move(number)), message_(std::move(message)) {  }

    std::string to_json_string() override {
        Json::Value root;
        root["user_id"] = send_to_user_number_;
        Json::Value messageArray(Json::arrayValue);
        Json::Value messageObj;
        messageObj["type"] = "text";
        Json::Value dataObj;
        dataObj["text"] = message_;
        messageObj["data"] = dataObj;
        messageArray.append(messageObj);
        root["message"] = messageArray;
        Json::StreamWriterBuilder writer;
        writer["indentation"] = "    ";
        return Json::writeString(writer, root);
    }

};