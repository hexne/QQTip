/********************************************************************************
* @Author : hexne
* @Date   : 2025/12/28 15:32:36
********************************************************************************/

module;
export module config;
import std;
import nlohmann.json;


struct ServerConfig {
    int port{};
    std::string token;
};

struct Config {
    std::string admin;
    std::string host;
    ServerConfig http;
    ServerConfig ws;

    void load_config(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error(std::format("can't open config file => {}", path));
        }

        nlohmann::json root;
        file >> root;

        admin = root.value("admin", "");
        host  = root.value("host", "");

        http.port  = root["http"].value("port", 0);
        http.token = root["http"].value("token", "");

        ws.port  = root["ws"].value("port", 0);
        ws.token = root["ws"].value("token", "");
    }

    void save_config(const std::string& path) const {
        nlohmann::json root;

        root["admin"] = admin;
        root["host"]  = host;

        root["http"] = {
            {"port",  http.port},
            {"token", http.token}
        };

        root["ws"] = {
            {"port",  ws.port},
            {"token", ws.token}
        };

        std::ofstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error(std::format("can't write config file => {}", path));
        }

        file << root.dump(4);
    }
};


export Config config;
