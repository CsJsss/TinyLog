#include "TinyLog/include/LogConfig.h"
#include "TinyLog/include/SynLog.h"
#include "TinyLog/include/AsynLog.h"
#include "TinyLog/include/Logging.h"
#include "TinyLog/include/ThreadInfo.h"
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

void func() {
    for (int i = 0; i < 1e5; i++) {
        auto curId = TinyLog::ThreadInfo::getTid();
        LOG_TRACE("TRACE hello world, current thread id  = %d\n", curId);
        LOG_DEBUG("DEBUG hello world, current thread id  = %d\n", curId);
        LOG_INFO("INFO hello world, current thread id = %d\n", curId);
        LOG_WARN("WARN hello world, current thread id  = %d\n", curId);
        LOG_ERROR("ERROR hello world, current thread id  = %d\n", curId);
    }
}

unique_ptr<TinyLog::AsynLog> asynLog;

/* 异步日志输出 */
void asynOutput(const char *_msg, size_t _len, size_t _kenLen = 0) {
    asynLog->append(_msg, _len, _kenLen);
}

void AsynLogger() {
    /* 设置日志等级和日志文件相关的设置 */
    TinyLog::LogConfig config;
    config.logLevel = TinyLog::Logger::TRACE;
    config.fileOption.baseName = "main.cpp-AsynLog";
    config.fileOption.fileWriter = TinyLog::NORMALFileWriter;
    /* 500MB */
    config.fileOption.rooSize = static_cast<size_t>(500 * 1024) * 1024;

    TinyLog::Logger::setConfig(config);

    /* 设置输出 */
    TinyLog::Logger::setOutput(asynOutput);

    asynLog =
        make_unique<TinyLog::AsynLog>();
    asynLog->start();
}


int main(int, char **) {
    auto start_time = chrono::system_clock::now();

    AsynLogger();

    std::vector<std::thread> threads(8);

    for (auto &t : threads)
        t = std::thread(func);

    for (auto &t : threads)
        t.join();

    auto end_time = chrono::system_clock::now();
    chrono::milliseconds cost_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    std::cout << "AsynLogger cost time = " << cost_time.count() << endl;
    return 0;
}
