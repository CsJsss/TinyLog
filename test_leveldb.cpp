#include "TinyLog/include/LogConfig.h"
#include "TinyLog/include/SynLog.h"
#include "TinyLog/include/AsynLog.h"
#include "TinyLog/include/Logging.h"
#include "TinyLog/include/ThreadInfo.h"
#include "include/LeveldbLog.h"
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
        LOG_TRACE("TRACE hello world, current idx = %d\n", i);
        LOG_DEBUG("DEBUG hello world, current idx = %d\n", i);
        LOG_INFO("INFO hello world, current idx = %d\n", i);
        LOG_WARN("WARN hello world, current idx = %d\n", i);
        LOG_ERROR("ERROR hello world, current idx = %d\n", i);
    }
}

unique_ptr<TinyLog::LeveldbLog> leveldbLog;

/* 同步日志输出 */
void leveldbOutput(const char *_msg, size_t _len, size_t _keyLen) {
    leveldbLog->append(_msg, _len, _keyLen);
}

void LeveldbLogger() {
    /* 设置日志等级和日志文件相关的设置 */
    TinyLog::LogConfig config;
    config.logLevel = TinyLog::Logger::TRACE;
    config.fileOption.baseName = "main.cpp-LeveldbLog";
    config.fileOption.flushInterval = 3;
    TinyLog::Logger::setConfig(config);
    /* 设置输出 */
    leveldbLog = make_unique<TinyLog::LeveldbLog>(config.fileOption.baseName, config.fileOption.flushInterval);

    TinyLog::Logger::setOutput(leveldbOutput);
}


int main(int, char **) {

    auto start_time = chrono::system_clock::now();

    LeveldbLogger();

    std::vector<std::thread> threads(8);

    for (auto &t : threads)
        t = std::thread(func);

    for (auto &t : threads)
        t.join();

    auto end_time = chrono::system_clock::now();
    chrono::milliseconds cost_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    std::cout << "LeveldbLogger cost time = " << cost_time.count() << endl;
    return 0;
}
