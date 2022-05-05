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
#include "include/AsynLog.h"
#include "include/LogFile.h"
#include "include/Logging.h"
#include <cassert>
#include <chrono>
#include <memory>
#include <mutex>
#include <type_traits>

namespace TinyLog {

/* 用于异步日志的config */
// LogConfig kLogConfig;

/* 设置config, config的优先级弱于AsynLog的构造函数,
 * 其会被AsynLog构造函数的参数覆盖 */
void AsynLog::setConfig(const LogConfig &_cfg) { kLogConfig = _cfg; }

AsynLog::AsynLog(const std::string &_basename, size_t _rollSize,
                 int _flushInterval, int _bufferNums,
                 FileWriterType _writerType)
    : basename_(_basename), rollSize_(_rollSize),
      flushInterval_(_flushInterval), started_(false), counter_(1),
      bufferSize_(_bufferNums), writerType_(_writerType), head(std::make_shared<BufferNode>()),
      tail(std::make_shared<BufferNode>()) {
  /* 初始化 head 和 tail 的指针域*/
  head->next_ = tail;
  tail->prev_ = head;
}

AsynLog::~AsynLog() {
  if (started_)
    stop();
  /* 回收缓冲区资源, 裸指针的时候需要这样做 */
  // BufferNodePtr cur = head;
  // while (cur != nullptr) {
  //   BufferNodePtr nxt = cur->next_;
  //   delete cur;
  //   cur = nxt;
  // }
}

AsynLog::BufferNodePtr AsynLog::newBufferNode() {
  auto cur = std::make_shared<BufferNode>();
  cur->buff_ = std::make_unique<Buffer>();
  return cur;
}

/* 前端和后端的唯一接口 */
void AsynLog::append(const char *_msg, size_t _len) {
  /* RAII lock */
  std::unique_lock<std::mutex> lock(mtx_);
  std::unique_ptr<Buffer> &curBuff = curBuffNode->buff_;

  if (curBuff->avail() > _len) {
    curBuff->append(_msg, _len);
    return;
  }
  /* 当前缓冲区写满了, 从环形缓冲区中移除该bufferNode */
  /* curPtr = curBuffNode */
  assert(curBuffNode == head->next_);
  /* 将当前缓冲区加入待落盘队列 */
  writeBufferNode.push_back(curBuffNode);
  removeHead();
  bufferSize_ -= 1;

  /* 如果无空余缓冲, 则新建缓冲区 BufferNode, 并加入环形缓冲区中 */
  if (bufferSize_ == 0) {
    auto newNode = newBufferNode();
    addTail(newNode);
    bufferSize_ += 1;
  }
  /* 更新 curBuffNode, 然后写入消息 */
  curBuffNode = head->next_;
  curBuffNode->buff_->append(_msg, _len);
  /* 唤醒后台线程进行落盘操作 */
  cv_.notify_one();
}

/* 后台日志线程 */
void AsynLog::threadFunc() {
  assert(started_ == true);
  assert(bufferSize_ > 0);
  counter_.countdown();

  /* 构建默认大小的环形缓冲区 */
  for (int i = 0; i < bufferSize_; i++) {
    auto cur = newBufferNode();
    addHead(cur);
  }

  /* 初始化curBuffNode, 让其指向head -> next */
  curBuffNode = head->next_;
  /* 日志文件写的具体实现 */
  LogFile fileWriter(basename_, rollSize_, writerType_);

  while (started_) {
    assert(writeBufferNode.size() == 0);
    assert(curBuffNode == head->next_);
    /* RAII lock block */
    {
      std::unique_lock<std::mutex> lock(mtx_);
      /* 如果写的太慢会被刷盘间隔唤醒, 唤醒后进行刷盘,
       * 不能用while循环判断缓冲区是否为空, 如果缓冲区一直为空, 则导致死锁*/
      if (writeBufferNode.empty())
        cv_.wait_for(lock, std::chrono::seconds(flushInterval_));

      if (writeBufferNode.empty() and curBuffNode->buff_->size() == 0)
        continue;
      /* 将当前缓冲区加入待落盘队列 */
      writeBufferNode.push_back(curBuffNode);
      removeHead();
      bufferSize_ -= 1;

      /* 如果之后无缓冲区则新建 */
      if (bufferSize_ == 0) {
        auto newNode = newBufferNode();
        addTail(newNode);
        bufferSize_ += 1;
      }
      /* 更新 curBuffNode */
      curBuffNode = head->next_;
    }

    /* 异步写入日志, 限制最大写入缓冲区数目*/
    int mxNums = std::min(static_cast<int>(writeBufferNode.size()),
                          kLogConfig.fileOption.maxBuffToWrite);
    for (int i = 0; i < mxNums; i++) {
      BufferNodePtr node = writeBufferNode[i];
      fileWriter.append(node->buff_->data(), node->buff_->size());
    }

    /* Flush */
    fileWriter.flush();

    /* RAII lock block*/
    {
      /* 归还buffer到环形缓冲区中 */
      std::unique_lock<std::mutex> lock(mtx_);
      for (auto &node : writeBufferNode) {
        /* 必须清空后加入环形缓冲区 */
        node->buff_->clear();
        addTail(node);
        bufferSize_ += 1;
      }
      writeBufferNode.clear();
    }
  }

  /* Last Flush */
  fileWriter.flush();
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