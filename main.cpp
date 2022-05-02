#include <iostream>
#include <thread>
#include <vector>
#include "TinyLog/include/logging.h"
#include "TinyLog/src/logging.cpp"

// using namespace TinyLog;

void func () {
    std::thread::id curId = std::this_thread::get_id();

    LOG_INFO("hello world, ThreadId = %d\n", curId);
}

int main(int, char**) {
    std::vector<std::thread> threads(16);

    for (auto& t : threads)
        t = std::thread(func);

    for (auto& t : threads)
        t.join();

    return 0;
}
