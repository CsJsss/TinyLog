/**
 * @file asynlog.h
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-05-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __TINYLOG_INCLUDE_ASYNLOG_H_
#define __TINYLOG_INCLUDE_ASYNLOG_H_

#include "include/CountDownLatch.h"
#include "include/buffer.h"
#include "include/logConfig.h"
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

class AsynLog : noncopyable {
public:
  AsynLog(const std::string &_basename = kLogConfig.fileOption.baseName,
          size_t _rollSize = kLogConfig.fileOption.rooSize,
          int _flushInterval = kLogConfig.fileOption.flushInterval,
          int _bufferNums = kLogConfig.fileOption.bufferNums);

  ~AsynLog() {
    if (started_)
      stop();
  }

  /* 静态方式设置LogConfig */
  static void setConfig(const LogConfig &);

  /* 前端和后端的唯一接口 */
  void append(const char *_msg, size_t _len);

  void start() {
    assert(started_ == false);
    started_ = true;
    thread_ = std::thread(&AsynLog::threadFunc, this);
    counter_.wait();
  }

  void stop() {
    assert(started_ == true);
    started_ = false;
    cv_.notify_one();
    thread_.join();
  }

private:
  void threadFunc();
  using Buffer = LogBuffer::FixedBuffer<LogBuffer::kBuffSize>;

  /* 日志文件 */
  const std::string basename_;
  const size_t rollSize_;
  const int flushInterval_;

  std::atomic_bool started_;
  std::thread thread_;
  CountDownLatch counter_;

  /* 用于临界区的处理和后台写日志线程的唤醒 */
  std::mutex mtx_;
  std::condition_variable cv_;

  /* 缓冲区结点, 用于实现 环形缓冲 */
  struct BufferNode {
    std::unique_ptr<Buffer> data_;
    BufferNode *prev_, *next_;
    BufferNode() : data_(nullptr), prev_(nullptr), next_(nullptr) {}
    BufferNode(Buffer *_data) : data_(_data), prev_(nullptr), next_(nullptr) {}
  };

  using BufferNodePtr = BufferNode *;

  /* 环形缓冲区头、尾结点(无数据, 用作哨兵) */
  BufferNodePtr head, tail;
  int bufferSize_;

  /* 操作缓冲区函数 */
  void addHead(BufferNodePtr);
  void addTail(BufferNodePtr);
  BufferNodePtr removeHead();
  BufferNodePtr removeTail();
  BufferNodePtr newBufferNode();

  /* 指向当前写入的BufferNode */
  BufferNodePtr curBuffNode;
  /* 后台线程要落盘的buffer, 已经写满或因刷盘间隔而被threadFunc放入
   * writrBuffer中 */
  std::vector<BufferNodePtr> writeBufferNode;
};

} // namespace TinyLog

#endif /* __TINYLOG_INCLUDE_ASYNLOG_H_ */
