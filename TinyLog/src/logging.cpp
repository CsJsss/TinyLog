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

#include "include/logging.h"
#include "include/logConfig.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <mutex>

namespace TinyLog {

/*Global Logger Config*/
LogConfig kLogConfig;

/* Logger 单例变量的定义 */
Logger *Logger::_logger = nullptr;
std::mutex Logger::_mtx;
thread_local time_t prevSecond;
thread_local char timeStr[64];

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
inline void defaultOutput(const char *_msg, size_t _len) {
  size_t n = fwrite(_msg, 1, _len, stdout);
  (void)n;
}

/* 默认flush函数*/
inline void defaultFlush() { fflush(stdout); }

Logger::LogLevel _global_logLevel = defaultLogLevel();
Logger::outPutFunc _global_outPutFunc = defaultOutput;
Logger::flushFunc _global_flushFunc = defaultFlush;

inline Logger::LogLevel Logger::getLogLevel() { return kLogConfig.logLevel; }

inline void Logger::setConfig(const LogConfig &_config) {
  kLogConfig = _config;
}

/* file和line在编译期获取其长度fileLen, lineLen */
template <typename... Args>
void Logger::append(const char *file, size_t fileLen, const char *line,
                    size_t lineLen, const char *fmt, Logger::LogLevel level,
                    Args &&...args) {
  thread_local Buffer buffer;
  /* 处理日志中的时间 */
  // TODO:

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

  /* 正文 */
  int n = snprintf(buffer.current(), buffer.avail() - 1, fmt, args...);
  buffer.addLen(static_cast<size_t>(n));

  /* appen到outPutfunc */
  _global_outPutFunc(buffer.data(), buffer.size());
  /* 缓冲区清空, 等待线程下一条日志记录, 因为其是TLS变量*/
  buffer.clear();
  /* 学习muduo的做法 */
  if (level == Logger::FATAL) {
    _global_flushFunc();
    abort();
  }
}

} // namespace TinyLog
