#ifndef __TINYLOG_INCLUDE_LEVELDBLOG_H_
#define __TINYLOG_INCLUDE_LEVELDBLOG_H_

#include "include/Buffer.h"
#include "include/CountDownLatch.h"
#include "include/FileWriter.h"
#include "include/LogConfig.h"
#include "include/LogFile.h"
#include "include/noncopyable.h"
#include "leveldb/db.h"
#include "leveldb/options.h"
#include "leveldb/write_batch.h"
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace TinyLog {

extern LogConfig kLogConfig;

class LeveldbLog : noncopyable {
public:
  LeveldbLog(const std::string _basename = kLogConfig.fileOption.baseName,
             int _flushInterval = kLogConfig.fileOption.flushInterval);

  ~LeveldbLog();

  /* 前端和后端的唯一接口 */
  void append(const char *_msg, size_t _len, size_t keyLen = 0);

private:
  void flush();

  /* 日志文件 */
  const std::string &basename_;
  const int flushInterval_;
  /* 用于临界区的处理, 互斥修改prevSecond */
  std::mutex mtx_;
  time_t prevSecond;

  /* 日志文件写的具体实现 */
  leveldb::DB *db_;
  leveldb::WriteBatch batch;
};

} // namespace TinyLog

#endif /* __TINYLOG_INCLUDE_LEVELDBLOG_H_ */
