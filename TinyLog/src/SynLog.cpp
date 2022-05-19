/**
 * @file asynlog.cpp
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-05-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "include/SynLog.h"
#include "include/LogFile.h"
#include "include/Logging.h"
#include "include/Timestamp.h"
#include <cassert>
#include <chrono>
#include <memory>
#include <mutex>
#include <type_traits>

namespace TinyLog {
/*
SynLog::SynLog(const std::string &_basename, size_t _rollSize,
               int _flushInterval, FileWriterType _writerType)
    : flushInterval_(_flushInterval), prevSecond(Timestamp::now().getSeconds()),
      fileWriter(_basename, _rollSize, _writerType) {}
*/
/* 前端和后端的唯一接口 */
void SynLog::append(const char *_msg, size_t _len, size_t keyLen) {
  /* RAII lock */
  std::unique_lock<std::mutex> lock(mtx_);
  fileWriter.append(_msg, _len);
  time_t now = Timestamp::now().getSeconds();
  if (static_cast<int>(now - prevSecond) >= flushInterval_) {
    fileWriter.flush();
    prevSecond = now;
  }
}

} // namespace TinyLog