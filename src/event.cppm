/********************************************************************************
* @Author : hexne
* @Date   : 2025/12/21 18:50:51
********************************************************************************/

module;
#include <httplib.h>
#include <json/json.h>
export module event;
import std;
import tools;
import time;

export struct Event {
    std::chrono::seconds interval{};
    virtual std::tuple<bool, std::string> check() = 0;
    virtual bool is_finish() = 0;
    virtual ~Event() = default;
};

export class NovelUpdate : public Event {
    std::string url_{};
    int last_chapter_number_ = -1;
public:
    explicit NovelUpdate(std::string url) : url_(std::move(url)) {  }

    std::tuple<bool, std::string> check() override {
        auto time = LocalTime::now();
        int cur_chapter_number = get_last_chapter_number(url_);
        if (last_chapter_number_ == -1) {
            last_chapter_number_ = cur_chapter_number;
            auto message = std::format("[ {} ]  添加小说更新提醒", time.get_clock_string(), cur_chapter_number - last_chapter_number_);
            return { true, message };
        }
        if (cur_chapter_number == -1) {
            return {true, "获取小说更新信息失败，请查看log"};
        }

        if (cur_chapter_number == last_chapter_number_)
            return {false, ""};

        auto message = std::format("[ {} ]  小说更新了{}章", time.get_clock_string(), cur_chapter_number - last_chapter_number_);
        last_chapter_number_ = cur_chapter_number;
        return { true, message};
    }
    bool is_finish() override {
        return false;
    }


};

export class Tip : Event {
    LocalTime tip_time_{};
    std::string message_;
    int repeat_count{};
    bool is_repeat_{};

public:
    Tip(LocalTime tip_time, bool is_reapeat) : tip_time_(tip_time), is_repeat_(is_reapeat) {  }

    Tip(LocalTime tip_time, int repeat_count) : tip_time_(tip_time), repeat_count(repeat_count) {  }

    Tip(LocalTime tip_time, std::string message) : tip_time_(tip_time), message_(std::move(message)) { }

    std::tuple<bool, std::string> check() override {
        if (tip_time_ < LocalTime::now())
            return { true, message_};
        return {false, ""};
    }

    bool is_finish() override {
        if (is_repeat_)
            return false;
        if (repeat_count > 0)
            return false;
        return true;
    }
};

export std::vector<std::shared_ptr<Event>> load_events() {
    using namespace std;

    vector<shared_ptr<Event>> ret;

    // 文件不存在 → 创建空 object {}
    if (!filesystem::exists("events.json")) {
        ofstream create("events.json");
        create << "{}";
        return ret;
    }

    ifstream in("events.json", ios::binary);
    if (!in) return ret;

    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;

    Json::Value root;
    string errs;

    bool ok = Json::parseFromStream(builder, in, &root, &errs);
    if (!ok) {
        std::println("JSON parse error: {}", errs);
        return ret;
    }

    if (!root.isObject()) {
        std::println("events.json 格式错误：根节点必须是 object");
        return ret;
    }

    // 遍历所有事件
    for (const auto& name : root.getMemberNames()) {
        const Json::Value& cfg = root[name];

        if (cfg["event-type"].asString() == "Tip") {

            // Tip tip;

        }
        else if (cfg["event-type"] == "NovelUpdate") {
            ret.emplace_back(make_shared<NovelUpdate>(cfg["url"].asString()));
        }
    }

    return ret;
}
