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
#include <unistd.h> /* gethostname */

namespace TinyLog {

LogFile::LogFile(const std::string &_basename, size_t _rollSize,
                 int _flushInterval, FileUtilType _type)
    : basename_(_basename), rollSize_(_rollSize),
      flushInterval_(_flushInterval) {}

std::string getHostName() {
  char buff[256];
  if (gethostname(buff, sizeof(buff)) == 0) {
    buff[sizeof(buff) - 1] = '\0';
    return buff;
  } else {
    return "UnKnownHost";
  }
}

std::string LogFile::getLogFileName(const std::string &_basename, time_t *now) {
  std::string fileName;
  fileName += _basename;

  char timeStr[32];
  struct tm tm;
  time(now);
  gmtime_r(now, &tm);
  strftime(timeStr, sizeof(timeStr), "-%Y%m%d-%H%M%S.", &tm);
  fileName += timeStr;

  fileName += getHostName();


  fileName += ".log";
  return fileName;
}

} // namespace TinyLog