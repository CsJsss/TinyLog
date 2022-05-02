/**
 * @file CountDownLatch.h
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-05-02
 *
 * @copyright Copyright (c) 2022
 *
 */

/*计数器类, cpp20后可以使用信号量*/

#include "include/noncopyable.h"
#include <condition_variable>
#include <mutex>

namespace TinyLog {

class CountDownLatch : noncopyable {
public:
  explicit CountDownLatch(int _count);

  void wait();

  void countdown();

private:
  /* 对于const对象也必须可以操作其mutex */
  mutable std::mutex mtx_;
  std::condition_variable cv_;
  int count_;
};

} // namespace TinyLog