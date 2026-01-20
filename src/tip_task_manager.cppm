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
        if (check_chapter == -1 or last_chapter_number_ == -1 or check_chapter == last_chapter_number_)
            return { false, "" };

        int diff = check_chapter - last_chapter_number_;
        last_chapter_number_ = check_chapter;
        return { true, std::format("[{}] 《{}》更新了{}章", LocalTime::now().get_clock_string(), novel_name_, diff) };
    }
    NovelUpdate(std::string name, const std::vector<std::string> &urls) :
        novel_name_(std::move(name)), urls_(urls) {  }

};


class Tip : TipTask {
    LocalTime tip_time_;
    LocalTime interval_;
    std::string tip_message_;

public:
    // 参数是某个时间点，不含日期
    Tip(LocalTime tip_time, LocalTime interval, std::string tip_message) : tip_time_(tip_time) {
        auto today = LocalTime::now().today();
        tip_time_ = today + tip_time;
        interval_ = interval;
        tip_message_ = tip_message;
    }

    std::tuple<bool, std::string> check() override {
        auto now = LocalTime::now();
        if (now < tip_time_)
            return {false, ""};
        tip_time_ += interval_;
        return {true, tip_message_};
    }


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
            throw std::runtime_error(std::format("unknown time type => {}", ch));
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
                if (!obj.contains("urls") or !obj["urls"].is_array())
                    throw std::format_error(std::format("tasks format error {}", task_name));

                for (auto& u : obj["urls"])
                    urls.push_back(u.get<std::string>());

                auto duration = string_to_duration(obj["interval"]);
                auto task = std::make_shared<NovelUpdate>(task_name, urls);
                auto task_id = timer_.add_repeat_task([=] {
                    auto [res, message_str] = task->check();
                    if (!res)
                        return;
                    auto message = Message(obj["user"], message_str);
                    send_private_message(message);
                }, duration);
                tasks_.insert({task_id, task});
            }

            // 普通Tip
            else if (type == "Tip") {

            }



        }
    }
    void save_task(std::string path) {

    }

    void add_task(int id, std::shared_ptr<TipTask> task) {  }
    void remove_task(int id) {  }
    void update_task(int id, std::shared_ptr<TipTask> task) {
        tasks_[id] = task;
    }

    auto&& search_task(int id) {
        return tasks_[id];
    }

    std::string tasks_info() {
        auto count = std::format("运行中的任务数量 : {}", timer_.task_count());
        auto novel_update = std::format("{}", "");
        auto tip = std::format("{}", "");
        return std::format("{}\n{}\n{}\n", count, novel_update, tip);
    }

    int count() const {
        return tasks_.size();
    }

};

export TipTaskManager tip_task_manager;