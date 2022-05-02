/**
 * @file logging.h
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-04-30
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __TINYLOG_INCLUDE_LOGGING_H_
#define __TINYLOG_INCLUDE_LOGGING_H_

#include "include/buffer.h"
#include "include/noncopyable.h"
#include <ctime>
#include <functional>
#include <mutex>

namespace TinyLog {

/* 前向声明 */
class LogConfig;

/* compile-time 计算字符串长度
ref-link:
https://stackoverflow.com/questions/15858141/conveniently-declaring-compile-time-strings-in-c
*/
class str_const { // constexpr string
private:
  const char *p_;
  std::size_t sz_;

public:
  template <std::size_t N>
  constexpr str_const(const char (&a)[N]) : p_(a), sz_(N - 1) {
    const char *idx = strrchr(p_, '/'); // builtin function
    if (idx) {
      p_ = idx + 1;
      sz_ -= static_cast<size_t>(p_ - a);
    }
  }

  constexpr char operator[](std::size_t n) { // []
    return n < sz_ ? p_[n] : throw std::out_of_range("");
  }

  constexpr std::size_t size() { return sz_; }
  constexpr const char *data() { return p_; }
};

/* compile-time 将x转换为字符串 */
#define strify_(x) val_(x)
#define val_(x) #x

/* 编译期获取字符串长度宏定义 */
#define getStrLen_(x) TinyLog::str_const(x).size()
/* 编译期获取字符串的宏定义 */
#define getStr_(x) TinyLog::str_const(x).data()

class Logger : noncopyable {
public:
  enum LogLevel {
    TRACE = 0,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    numOfLevels,
  };

  /* 日志行缓冲 */
  using Buffer = LogBuffer::FixedBuffer<LogBuffer::kLineBuff>;

  /* Logger的输出函数*/
  using outPutFunc = std::function<void(const char *, size_t)>;
  /* Logger的落盘函数*/
  using flushFunc = std::function<void()>;

  /* 设置Logger的输出函数 */
  static void setOutput(outPutFunc);
  /* 设置Logger的落盘函数 */
  static void setFlush(flushFunc);

  /* 获取Logger的日志等级 */
  static LogLevel getLogLevel();
  /* 设置Logger的日志等级 */
  static void setLogLevel(LogLevel level);

  /* 获取Logger类单例对象 */
  static Logger *getInstance();

  /* 设置LogConfig */
  static void setConfig(const LogConfig &);

  /* 供LOG_*系列宏使用, 将一条日志记录存入LineBuffer中 */
  template <typename... Args>
  void append(const char *file, size_t fileLen, const char *line,
              size_t lineLen, const char *fmt, Logger::LogLevel level,
              Args &&...args);

private:
  /* Logger 单例变量的声明 */
  static Logger *_logger;
  static std::mutex _mtx;
  Logger() = default;
  ~Logger() = default;

  /* 线程局部变量, 对日期和时间部分进行缓存, 每个线程拥有独立的缓存*/
  static thread_local time_t prevSecond;
  static thread_local char timeStr[64];
  // static thread_local Buffer buffer;
};

extern Logger::outPutFunc _global_outPutFunc;
extern Logger::flushFunc _global_flushFunc;
extern LogConfig kLogConfig;

} // namespace TinyLog

/* LOG_* 相关的宏定义, 向用户提供接口 */
#define LOG_TRACE(fmt, args...)                                                \
  do {                                                                         \
    if (TinyLog::Logger::getLogLevel() <= TinyLog::Logger::TRACE) {            \
      TinyLog::Logger::getInstance()->append(                                  \
          getStr_(__FILE__), getStrLen_(__FILE__), strify_(__LINE__),          \
          getStrLen_(strify_(__LINE__)), fmt, TinyLog::Logger::TRACE, args);   \
    }                                                                          \
  } while (0)

#define LOG_DEBUG(fmt, args...)                                                \
  do {                                                                         \
    if (TinyLog::Logger::getLogLevel() <= TinyLog::Logger::DEBUG) {            \
      TinyLog::Logger::getInstance()->append(                                  \
          getStr_(__FILE__), getStrLen_(__FILE__), strify_(__LINE__),          \
          getStrLen_(strify_(__LINE__)), fmt, TinyLog::Logger::DEBUG, args);   \
    }                                                                          \
  } while (0)

#define LOG_INFO(fmt, args...)                                                 \
  TinyLog::Logger::getInstance()->append(                                      \
      getStr_(__FILE__), getStrLen_(__FILE__), strify_(__LINE__),              \
      getStrLen_(strify_(__LINE__)), fmt, TinyLog::Logger::INFO, args)
#define LOG_WARN(fmt, args...)                                                 \
  TinyLog::Logger::getInstance()->append(                                      \
      getStr_(__FILE__), getStrLen_(__FILE__), strify_(__LINE__),              \
      getStrLen_(strify_(__LINE__)), fmt, TinyLog::Logger::WARN, args)
#define LOG_ERROR(fmt, args...)                                                \
    TinyLog::Logger::getInstance()->append(getStr_(__FILE__), getStrLen_(__FILE__), strify_(__LINE__), getStrLen_(strify_(__LINE__), fmt, TinyLog::Logger::ERROR, args)
#define LOG_FATAL(fmt, args...)                                                \
  TinyLog::Logger::getInstance()->append(                                      \
      getStr_(__FILE__), getStrLen_(__FILE__), strify_(__LINE__),              \
      getStrLen_(strify_(__LINE__)), fmt, TinyLog::Logger::FATAL, args)

#endif /* __TINYLOG_INCLUDE_LOGGING_H_ */
