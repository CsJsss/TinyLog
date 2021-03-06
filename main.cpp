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
    asynLog->append(_msg, _len);
}

void setLogger() {
    TinyLog::Logger::setOutput(asynOutput);
    /* 128 MB */
    int RollSize = 128 * 1024 * 1024;
    TinyLog::FileWriterType writerType = TinyLog::NORMALFileWriter;
    asynLog =
        make_unique<TinyLog::AsynLog>("main.cpp", RollSize, 3, 4, writerType);
    asynLog->start();
}

void setLoggerByConfig() {
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

unique_ptr<TinyLog::SynLog> synLog;

/* 同步日志输出 */
void synOutput(const char *_msg, size_t _len, size_t _keyLen) {
    synLog->append(_msg, _len);
}

void SynLogger() {
    /* 设置日志等级和日志文件相关的设置 */
    TinyLog::LogConfig config;
    config.logLevel = TinyLog::Logger::TRACE;
    config.fileOption.baseName = "main.cpp-SynLog";
    config.fileOption.fileWriter = TinyLog::NORMALFileWriter;
    /* 500MB */
    config.fileOption.rooSize = static_cast<size_t>(500 * 1024) * 1024;
    TinyLog::Logger::setConfig(config);
    
    synLog = make_unique<TinyLog::SynLog>();
    /* 设置输出 */
    TinyLog::Logger::setOutput(synOutput);
}

void testGenerate() {
    /* 设置outPutFunc 和 flushFunc 为空, 测试日志行的生成速度 */
    TinyLog::Logger::setOutput(nullptr);
    TinyLog::Logger::setFlush(nullptr);
}


int main(int, char **) {
    auto start_time = chrono::system_clock::now();
    // setLogger();
    setLoggerByConfig();
    // SynLogger();
    // testGenerate();

    std::vector<std::thread> threads(8);

    for (auto &t : threads)
        t = std::thread(func);

    for (auto &t : threads)
        t.join();

    auto end_time = chrono::system_clock::now();
    chrono::milliseconds cost_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    std::cout << "cost time = " << cost_time.count() << endl;
    return 0;
}
