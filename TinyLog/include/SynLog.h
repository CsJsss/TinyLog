/**
 * @file SynLog.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-05-05
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __TINYLOG_INCLUDE_SYNLOG_H_
#define __TINYLOG_INCLUDE_SYNLOG_H_

#include "include/Buffer.h"
#include "include/CountDownLatch.h"
#include "include/FileWriter.h"
#include "include/LogConfig.h"
#include "include/LogFile.h"
#include "include/noncopyable.h"
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace TinyLog {

extern LogConfig kLogConfig;

class SynLog : noncopyable {
public:
  SynLog(const std::string _basename = kLogConfig.fileOption.baseName,
         size_t _rollSize = kLogConfig.fileOption.rooSize,
         int _flushInterval = kLogConfig.fileOption.flushInterval,
         FileWriterType _writerType = kLogConfig.fileOption.fileWriter)
      : basename_(_basename), rollSize_(_rollSize),
        flushInterval_(_flushInterval), writerType_(_writerType),
        fileWriter(basename_, rollSize_, writerType_) {
    prevSecond = static_cast<time_t>(0);
  }

  ~SynLog() { fileWriter.flush(); }

  /* 前端和后端的唯一接口 */
  void append(const char *_msg, size_t _len, size_t keyLen = 0);

private:
  /* 日志文件 */
  const std::string &basename_;
  const size_t rollSize_;
  const int flushInterval_;
  FileWriterType writerType_;
  time_t prevSecond;
  /* 用于临界区的处理*/
  std::mutex mtx_;

  /* 日志文件写的具体实现 */
  LogFile fileWriter;
};

} // namespace TinyLog

#endif /* __TINYLOG_INCLUDE_SYNLOG_H_ */
