/**
 * @file Timestamp.h
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-05-03
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __INCLUDE_TIMESTAMP_H_
#define __INCLUDE_TIMESTAMP_H_

#include <cstdint>
#include <ctime>
#include <string>

namespace TinyLog {

class Timestamp {
public:
  Timestamp() : milliSecondsSinceEpoch_(0) {}
  explicit Timestamp(int64_t _milliSecondsSinceEpoch)
      : milliSecondsSinceEpoch_(_milliSecondsSinceEpoch) {}

  /* Unix 时间戳 */
  static Timestamp now();

  /* 格式化时间: 使用localtime_r保证线程安全 */
  std::string toString();

  /* 获取毫秒 */
  int64_t getMilliSeconds() { return milliSecondsSinceEpoch_; }

  /* 获取秒 */
  time_t getSeconds() {
    return static_cast<time_t>(milliSecondsSinceEpoch_ /
                               kmilliSecondsPerSecond);
  }

  /* 时间戳类的静态数据成员 */
  static constexpr int kmilliSecondsPerSecond = 1000;

private:
  /* 毫秒级别的精度*/
  int64_t milliSecondsSinceEpoch_;
};

} // namespace TinyLog

#endif /* __INCLUDE_TIMESTAMP_H_ */
