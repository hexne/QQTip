/********************************************************************************
* @Author : hexne
* @Date   : 2025/12/22 19:28:02
********************************************************************************/

module;
#include <json/json.h>
#include <httplib.h>
export module tools;
import std;
import message;
import time;

struct Config {
    std::string host;
    int port;
    std::string token;
} config;

export void load_config() {
    const std::string& filename = "config.json";
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开配置文件: " + filename);
    }

    Json::Value root;
    Json::CharReaderBuilder readerBuilder;
    std::string errs;

    bool parsingSuccessful = Json::parseFromStream(readerBuilder, file, &root, &errs);
    file.close();

    if (!parsingSuccessful) {
        throw std::runtime_error("解析 JSON 失败: " + errs);
    }

    config.host = root["host"].asString();
    config.port = root["port"].asInt();
    config.token = root["token"].asString();
}


export bool send_message(const std::string& send_path, Message& message) {
    httplib::Client cli(config.host, config.port);
    httplib::Headers headers = {
        {"Authorization", config.token}
    };

    auto res = cli.Post(send_path, headers, message.to_json_string(), "application/json");
    if (res)
        return true;
    std::cerr << "Request failed: " << httplib::to_string(res.error()) << std::endl;
    return false;
}

export bool send_private_message(Message& message) {
    return send_message("/send_private_msg", message);
}

std::string get_command_res_string(std::string command) {
    auto pfile = popen(command.data(),"r");
    if (!pfile)
        throw std::runtime_error("popen() failed!");
    std::string ret;
    char buffer[2048] = "";
    while(fgets(buffer,sizeof(buffer),pfile))
        ret += buffer;
    fclose(pfile);
    return ret;
}

std::string get_html(const std::string &url) {
    return get_command_res_string("curl -s " + url);
}

export void log(const std::string &info) {
    LocalTime time = LocalTime::now();
    auto time_info = std::format("[ {} ]", time.get_string());
    std::ofstream log_file("./log", std::ios::app);
    log_file << std::format("{}\n{}\n\n", time_info, info);
}

export int get_last_chapter_number(const std::string &url) {
    auto html = get_html(url);
    std::regex regex(R"(第(\d+)章)");
    auto begin = std::sregex_iterator(html.begin(), html.end(), regex);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) {
        const auto& m = *it;
        std::string number = m[1].str();
        int chapter = std::stoi(number);
        return chapter;
    }
    log("没有查找到章节信息, html信息如下：\n" + html);
    return -1;
}
