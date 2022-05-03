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
#include "include/asynlog.h"
#include "include/logging.h"
#include <cassert>
#include <chrono>
#include <memory>
#include <mutex>
#include <type_traits>

namespace TinyLog {

/* 用于异步日志的config */
LogConfig kLogConfig;

/* 设置config, config的优先级弱于AsynLog的构造函数,
 * 其会被AsynLog构造函数的参数覆盖 */
void AsynLog::setConfig(const LogConfig &_cfg) { kLogConfig = _cfg; }

AsynLog::AsynLog(const std::string &_basename, size_t _rollSize,
                 int _flushInterval, int _bufferNums)
    : basename_(_basename), rollSize_(_rollSize),
      flushInterval_(_flushInterval), started_(false), counter_(1) {
  for (int i = 0; i < bufferSize_; i++) {
    auto cur = newBufferNode();
    addHead(cur);
  }
}

AsynLog::BufferNodePtr AsynLog::newBufferNode() {
  auto cur = new BufferNode;
  cur->data_ = std::make_unique<Buffer>();
  return cur;
}

/* 前端和后端的唯一接口 */
void AsynLog::append(const char *_msg, size_t _len) {
  /* RAII lock */
  std::unique_lock<std::mutex> lock(mtx_);
  std::unique_ptr<Buffer> &curBuff = curBuffNode->data_;

  if (curBuff->avail() > _len) {
    curBuff->append(_msg, _len);
    return;
  }
  /* 当前缓冲区写满了, 从环形缓冲区中移除该bufferNode */
  /* curPtr = curBuffNode */
  auto curPtr = removeHead();
  writeBufferNode.push_back(curPtr);

  /* 如果无空余缓冲, 则新建缓冲区 BufferNode, 并加入环形缓冲区中 */
  if (curBuffNode->next_ == tail) {
    auto newNode = newBufferNode();
    addTail(newNode);
    /* 更新 curBuffNode, 然后写入消息 */
    curBuffNode = newNode;
  }
  curBuffNode->data_->append(_msg, _len);
  /* 唤醒后台线程进行落盘操作 */
  cv_.notify_one();
}

/* 后台日志线程 */
void AsynLog::threadFunc() {
  assert(started_ == true);
  counter_.countdown();

  /* 日志文件写的具体实现 */

  while (started_) {
    assert(writeBufferNode.size() == 0);
    /* RAII lock block */
    {
      std::unique_lock<std::mutex> lock(mtx_);
      /* 如果写的太慢会被刷盘间隔唤醒, 唤醒后进行刷盘 */
      if (writeBufferNode.empty())
        cv_.wait_for(lock, std::chrono::seconds(flushInterval_));

      /* 将当前缓冲区加入待落盘队列 */
      writeBufferNode.push_back(curBuffNode);
      /* 如果之后无缓冲区则新建 */
      if (curBuffNode->next_ == tail) {
        auto newNode = newBufferNode();
        addTail(newNode);
        /* 更新 curBuffNode */
        curBuffNode = newNode;
      }
    }

    /* 异步写入日志 */
    for (auto &node : writeBufferNode) {
      // TODO: LogFile.append
    }

    /* RAII lock block*/
    {
      /* 归还buffer到环形缓冲区中 */
      std::unique_lock<std::mutex> lock(mtx_);
      for (auto &node : writeBufferNode)
        addTail(node);
      writeBufferNode.clear();
    }

    /* Flush */
    // TODO: LogFile.flush
  }
  /* Last Flush */
  // TODO: LogFile.flush
}

/* 必须在临界区中执行, 即被 mutex 保护 */
void AsynLog::addHead(BufferNodePtr cur) {
  cur->next_ = head->next_;
  head->next_->prev_ = cur;
  head->next_ = cur;
  cur->prev_ = head;
}

/* 必须在临界区中执行, 即被 mutex 保护 */
AsynLog::BufferNodePtr AsynLog::removeHead() {
  assert(bufferSize_ != 0);
  BufferNodePtr ret = head->next_;
  head->next_ = head->next_->next_;
  head->next_->prev_ = head;
  return ret;
}

/* 必须在临界区中执行, 即被 mutex 保护 */
void AsynLog::addTail(BufferNodePtr cur) {
  cur->prev_ = tail->prev_;
  tail->prev_->next_ = cur;
  cur->next_ = tail;
  tail->prev_ = cur;
}

/* 必须在临界区中执行, 即被 mutex 保护 */
AsynLog::BufferNodePtr AsynLog::removeTail() {
  assert(bufferSize_ != 0);
  BufferNodePtr ret = tail->prev_;
  tail->prev_ = tail->prev_->prev_;
  tail->prev_->next_ = tail;
  return ret;
}

} // namespace TinyLog