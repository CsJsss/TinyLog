/**
 * @file LogFile.cpp
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-05-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "include/LogFile.h"
#include "include/FileWriter.h"
#include "include/ThreadInfo.h"
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <string>
#include <unistd.h> /* gethostname */

namespace TinyLog {

LogFile::LogFile(const std::string &_basename, size_t _rollSize,
                 FileWriterType _fileWriterType)
    : basename_(_basename), rollSize_(_rollSize),
      fileWriterType_(_fileWriterType) {
  rollFile();
}

/* 仅用于日志后台线程, 因此无需加锁 */
void LogFile::append(const char *_msg, size_t _len) {
  file_->append(_msg, _len);
  if (file_->writtenBytes() > rollSize_)
    rollFile();
}

/* 仅用于日志后台线程, 因此无需加锁 */
void LogFile::flush() { file_->flush(); }

void LogFile::rollFile() {
  std::string newFileName = getLogFileName(basename_);
  if (fileWriterType_ == NORMALFileWriter)
    file_ = std::make_unique<NormalFileWriter>(newFileName);
  else {
    fprintf(stderr, "UnknowFileWriterType \n");
    abort();
  }
}

std::string getHostName() {
  char buff[256];
  if (gethostname(buff, sizeof(buff)) == 0) {
    buff[sizeof(buff) - 1] = '\0';
    return buff;
  } else {
    return "UnKnownHost";
  }
}

std::string LogFile::getLogFileName(const std::string &_basename) {
  std::string fileName;
  fileName += _basename;

  char timeStr[32];
  struct tm tm_time;
  time_t now = 0;
  time(&now);
  /* localtime 本地时间 */
  localtime_r(&now, &tm_time);
  strftime(timeStr, sizeof(timeStr), "-%Y%m%d-%H%M%S.", &tm_time);
  fileName += timeStr;

  fileName += getHostName();
  fileName += '.' + ThreadInfo::getPidStr();

  fileName += ".log";
  return fileName;
}

} // namespace TinyLog