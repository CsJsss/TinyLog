/**
 * @file CountDownLatch.cpp
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-05-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __TINYLOG_SRC_COUNTDOWNLATCH_H_
#define __TINYLOG_SRC_COUNTDOWNLATCH_H_

#include "include/CountDownLatch.h"
#include <mutex>

namespace TinyLog {
/* 构造函数 */
CountDownLatch::CountDownLatch(int _count) : count_(_count) {}

void CountDownLatch::wait() {
  std::unique_lock<std::mutex> lock(mtx_);
  while (count_ > 0)
    cv_.wait(lock);
}

void CountDownLatch::countdown() {
  std::unique_lock<std::mutex> lock(mtx_);
  if (--count_ == 0)
    cv_.notify_all();
}

} // namespace TinyLog

#endif /* __TINYLOG_SRC_COUNTDOWNLATCH_H_ */
