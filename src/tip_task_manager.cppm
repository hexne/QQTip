/********************************************************************************
* @Author : hexne
* @Date   : 2025/12/29 13:44:24
********************************************************************************/
module;
#include <cstdio>
export module tip_task_manager;
import std;
import time;
import timer;
import nlohmann.json;
import message;

struct TipTask {
    virtual std::tuple<bool, std::string> check() = 0;

    virtual ~TipTask() = default;
};


class NovelUpdate : public TipTask {
    std::string novel_name_{};
    std::vector<std::string> urls_{};
    int last_chapter_number_ = -1;

    static std::string get_command_out(const std::string &command) {
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

    static int get_latest_chapter_number(const std::string &url) {
        auto html = get_command_out("curl -s " + url);
        std::regex regex(R"(第([0-9]{1,6})章)");
        auto begin = std::sregex_iterator(html.begin(), html.end(), regex);
        auto end = std::sregex_iterator();
        int max = -1;
        for (auto it = begin; it != end; ++it) {
            const auto& m = *it;
            std::string number = m[1].str();
            int chapter = std::stoi(number);
            if (chapter > max)
                max = chapter;
        }
        return max;
    }


public:
    std::tuple<bool, std::string> check() override {
        int check_chapter = -1;
        for (const auto& url : urls_) {
            auto current_chapter = get_latest_chapter_number(url);
            if (current_chapter > check_chapter)
                check_chapter = current_chapter;
        }
        if (check_chapter == -1 or check_chapter == last_chapter_number_)
            return { false, "" };

        int diff = check_chapter - last_chapter_number_;
        last_chapter_number_ = check_chapter;
        return { true, std::format("[{}] 《{}》更新了{}章", LocalTime::now().get_clock_string(), novel_name_, diff) };
    }
    NovelUpdate(std::string name, const std::vector<std::string> &urls) :
        novel_name_(std::move(name)), urls_(urls) {  }

};





export class TipTaskManager {
    Timer timer_;
    std::map<int, std::shared_ptr<TipTask>> tasks_;

    static std::chrono::seconds string_to_duration(std::string str) {
        int number = std::stoi(std::string(str.begin(), str.end() - 1));
        char ch = str.back();
        switch (ch) {
        case 's':
        case 'S':
            return std::chrono::seconds{number};
        case 'm':
        case 'M':
            return std::chrono::minutes{number};
        case 'h':
        case 'H':
            return std::chrono::hours{number};
        case 'd':
        case 'D':
            return std::chrono::days{number};
        default:
            throw std::runtime_error(std::format("unknown interval type => {}", ch));
        }


    }
public:
    void load_task(std::string path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error(std::format("can't open task file => {}", path));
        }
        nlohmann::json root;
        file >> root;

        for (auto& item : root.items()) {
            const std::string& task_name = item.key();
            auto& obj = item.value();

            std::string type = obj.value("event-type", "");

            // 小说更新
            if (type == "NovelUpdate") {
                std::vector<std::string> urls;
                if (!obj.contains("url") or !obj["url"].is_array()) {
                    throw std::format_error(std::format("tasks format error {}", task_name));
                    for (auto& u : obj["url"]) {
                        urls.push_back(u.get<std::string>());
                    }
                }
                auto duration = string_to_duration(obj["interval"]);
                auto task = std::make_shared<NovelUpdate>(task_name, urls);
                auto task_id = timer_.add_repeat_task([=] {
                    auto [res, message_str] = task->check();
                    if (!res)
                        return;
                    auto message = Message(obj["user"], message_str);
                    send_private_message(message);
                }, duration);


            }


            // 普通Tip
            else if (type == "Tip") {

            }



        }
    }
    void save_task(std::string path) {

    }

    void add_task(std::shared_ptr<TipTask> task) {  }
    void remove_task(std::shared_ptr<TipTask> task) {  }
    void update_task(std::shared_ptr<TipTask> task) {  }
    void search_task(std::shared_ptr<TipTask> task) {  }


};

export TipTaskManager tip_task_manager;