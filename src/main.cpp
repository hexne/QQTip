import std;
import message;
import event;
import tools;
import timer;


// 整个参数添加, 还得有进程间通信，让后台运行的进程A知道进程B添加了新的通知事件
int main(int argc, char *argv[]) {
    load_config();
    auto events = load_events();

    Timer timer;

    for (auto event : events) {
        timer.add_repeat_task([=] {
            auto [res, message] = event->check();
            if (!res)
                return;

            TextMessage send_message("1392894180", message);
            send_private_message(send_message);

        }, std::chrono::minutes {1});
    }


    while (!timer.is_finish())
        ;




    // while (true) {
    //
    //     for (auto &event : events) {
    //         if (const auto &[res, message] = event->check(); res) {
    //             TextMessage text_message("1392894180", message);
    //             send_message("/send_private_msg", text_message);
    //         }
    //     }
    //     std::this_thread::sleep_for(std::chrono::minutes(1));
    // }

}

