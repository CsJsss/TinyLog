#include "TinyLog/include/SynLog.h"
#include "TinyLog/include/AsynLog.h"
#include "TinyLog/include/Logging.h"
#include "TinyLog/include/ThreadInfo.h"
#include <chrono>
#include <cstddef>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <sys/syscall.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace std;

constexpr int cnt = 1e8;

void func() {
    for (int i = 0; i < cnt; i++) {
        auto curId = TinyLog::ThreadInfo::getTid();
        LOG_TRACE("TRACE hello world, current thread id  = %d\n", curId);
        LOG_DEBUG("DEBUG hello world, current thread id  = %d\n", curId);
        LOG_INFO("INFO hello world, current thread id = %d\n", curId);
        LOG_WARN("WARN hello world, current thread id  = %d\n", curId);
        LOG_ERROR("ERROR hello world, current thread id  = %d\n", curId);
    }
}


void testGenerate() {
    /* 设置outPutFunc 和 flushFunc 为空, 测试日志行的生成速度 */
    TinyLog::Logger::setOutput(nullptr);
    TinyLog::Logger::setFlush(nullptr);
}


int main(int, char **) {
    auto start_time = chrono::system_clock::now();

    testGenerate();

    std::vector<std::thread> threads(1);

    for (auto &t : threads)
        t = std::thread(func);

    for (auto &t : threads)
        t.join();

    auto end_time = chrono::system_clock::now();
    chrono::nanoseconds cost_time = chrono::duration_cast<chrono::nanoseconds>(end_time - start_time);

    std::cout << "Log Line Generation Cost Time = " << cost_time.count() << " ns, Average Cost Time = " << static_cast<double>(cost_time.count()) / cnt << " ns/item."<< endl;
    return 0;
}
