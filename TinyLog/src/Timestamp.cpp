/**
 * @file Timestamp.cpp
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-05-03
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "include/Timestamp.h"
#include "sys/time.h"
#include <cstddef>
#include <cstdio>
#include <ctime>

namespace TinyLog {

/* Unix 时间戳 */
Timestamp Timestamp::now() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  int64_t seconds = tv.tv_sec;
  return Timestamp(seconds * kmilliSecondsPerSecond +
                   tv.tv_usec / kmilliSecondsPerSecond);
}

/* 格式化时间: 使用localtime_r保证线程安全 */
std::string Timestamp::toString() {
  char buff[64];
  time_t second =
      static_cast<time_t>(milliSecondsSinceEpoch_ / kmilliSecondsPerSecond);
  struct tm tm_time;
  /* stores the data in a user-supplied struct tm_time*/
  localtime_r(&second, &tm_time);
  /* 年月日 时分秒 毫秒*/
  int millsecond =
      static_cast<int>(milliSecondsSinceEpoch_ % kmilliSecondsPerSecond);
  snprintf(buff, sizeof(buff), "%4d%02d%02d %02d:%02d:%02d.%03d",
           tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
           tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, millsecond);

  return buff;
}

} // namespace TinyLog