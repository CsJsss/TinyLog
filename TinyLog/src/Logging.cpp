/**
 * @file logging.cpp
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-04-30
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "include/Logging.h"
#include "include/Buffer.h"
#include "include/LogConfig.h"
#include "include/ThreadInfo.h"
#include "include/Timestamp.h"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <mutex>
#include <utility>

namespace TinyLog {

/*Global Logger Config*/
LogConfig kLogConfig;

/* Logger 单例变量的定义 */
Logger *Logger::_logger = nullptr;
std::mutex Logger::_mtx;

/* 用于回收单例资源 */
Logger::GC gcVariabel;

/* 线程局部变量, 对日期和时间部分进行缓存, 每个线程拥有独立的缓存*/
thread_local time_t prevSecond;
thread_local char timeStr[64];
thread_local Logger::Buffer buffer;

Logger *Logger::getInstance() {
  /* 双检锁实现单例模式 */
  if (_logger == nullptr) {
    /* RAII lock template */
    std::unique_lock<std::mutex> lock(_mtx);
    if (_logger == nullptr)
      _logger = new Logger;
  }
  return _logger;
}

/* 默认日志等级为: INFO*/
inline Logger::LogLevel defaultLogLevel() { return kLogConfig.logLevel; }

const char *LogLevelStr[Logger::LogLevel::numOfLevels] = {
    "TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL",
};

/* 默认输出路径为stdout */
inline void defaultOutput(const char *_msg, size_t _len, size_t _keyLen = 0) {
  size_t n = fwrite(_msg, 1, _len, stdout);
  (void)n;
}

/* 默认flush函数*/
inline void defaultFlush() { fflush(stdout); }

Logger::LogLevel _global_logLevel = defaultLogLevel();
Logger::outPutFunc _global_outPutFunc = defaultOutput;
Logger::flushFunc _global_flushFunc = defaultFlush;

void Logger::setOutput(Logger::outPutFunc _func) {
  _global_outPutFunc = std::move(_func);
}

void Logger::setFlush(Logger::flushFunc _func) {
  _global_flushFunc = std::move(_func);
}

/* 不能被内联 */
Logger::LogLevel Logger::getLogLevel() { return kLogConfig.logLevel; }

/* 不能被内联 */
void Logger::setConfig(const LogConfig &_config) { kLogConfig = _config; }

/* 设置日志时间, 使用TLS进行缓存优化 */
inline void Logger::formatTime() {
  Timestamp current = Timestamp::now();
  time_t curSecond = current.getSeconds();
  int milliSecond = static_cast<int>(current.getMilliSeconds() %
                                     Timestamp::kmilliSecondsPerSecond);
  /* 无法使用TLS时间缓存进行优化 */
  if (curSecond != prevSecond) {
    prevSecond = curSecond;
    /* 重新写入时间到TLS中 */
    struct tm tm_time;
    localtime_r(&curSecond, &tm_time);

    snprintf(timeStr, sizeof(timeStr), "%4d-%02d-%02d %02d:%02d:%02d-",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
  }
  /* 写入日志行缓冲区中: 精确到秒 */
  buffer.append(timeStr, 20);
  /* 写入毫秒 */
  char mStr[6];
  snprintf(mStr, sizeof(mStr), "%03d ", milliSecond);
  buffer.append(mStr, 4);
}

/* file和line在编译期获取其长度fileLen, lineLen */
void Logger::append(const char *file, size_t fileLen, const char *line,
                    size_t lineLen, const char *fmt, Logger::LogLevel level,
                    ...) {
  /* 处理日志中的时间 */
  formatTime();

  /* 添加线程号: ID*/
  buffer.append("tid:", 4);
  buffer.append(ThreadInfo::getTidStr(), ThreadInfo::getTidStrlen());

  /* 源文件[line:行号] - 日志级别
   *  example:
   *  test.py[line:233] - INFO: 正文
   */
  buffer.append(file, fileLen);
  buffer.append("[line:", 6);
  buffer.append(line, lineLen);
  buffer.append("] - ", 4);
  buffer.append(LogLevelStr[level], 5);
  buffer.append(": ", 2);

  // size_t keyLen = buffer.size();
  /* 正文, 使用va_list、va_arg和va_end等宏来处理, 其原理是
   * cdelc_ 参数从右到左压栈, 第一个参数在栈顶 */
  va_list argPtr;
  va_start(argPtr, fmt);
  int n = vsnprintf(buffer.current(), buffer.avail(), fmt, argPtr);
  va_end(argPtr);
  buffer.addLen(static_cast<size_t>(n));

  /* appen到outPutfunc */
  /* used for BenchMark*/
  // if (_global_outPutFunc != nullptr)
  size_t keyLen = buffer.size();
    _global_outPutFunc(buffer.data(), buffer.size(), keyLen);
  assert(buffer.size() < static_cast<size_t>(LogBuffer::kLineBuff));
  /* 缓冲区清空, 等待线程下一条日志记录, 因为其是TLS变量*/
  buffer.clear();
  /* 学习muduo的做法 */
  if (level == Logger::FATAL) {
    // if (_global_flushFunc)
      _global_flushFunc();
    abort();
  }
}

} // namespace TinyLog
