import std;
import config;
import listen;
import tip_task_manager;

int main(int argc, char *argv[]) {

    config.load_config("config.json");

    tip_task_manager.load_task("tasks.json");


    listen();


    while (true) {

    }




}

