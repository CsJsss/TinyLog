/**
 * @file buffer.h
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-05-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef __TINYLOG_INCLUDE_BUFFER_H_
#define __TINYLOG_INCLUDE_BUFFER_H_

#include "noncopyable.h"
#include <cstddef>
#include <cstring>

/* cpp17 features */
namespace TinyLog::LogBuffer {

/* 日志记录行缓冲区大小: 1Kb*/
const int kLineBuff = 1024;
/* 异步日志单个缓冲区大小: 4Mb */
const int kBuffSize = 4 * 1024 * 1024;

template <size_t SIZE> class FixedBuffer : noncopyable {
public:
  FixedBuffer() : _cur(0) {}

  ~FixedBuffer() = default;

  size_t size() const { return _cur; }

  void clear() {
    _cur = 0;
    // TODO: do we need this operation ?
    // memset(_data, 0, sizeof(_data));?
  }

  size_t avail() const { return SIZE - _cur; }

  const char *data() const { return _data; }

  /* 从外部直接写入内容到buffer中 */
  char *current() { return _data + _cur; }

  void addLen(size_t len) { _cur += len; }

  void append(const char *_msg, size_t _len) {
    if (avail() > _len) {
      memcpy(_data + _cur, _msg, _len);
      _cur += _len;
    }
  }

private:
  char _data[SIZE]; /* 缓冲区 */
  size_t _cur;      /* 第一个可写入位置 */
};

} // namespace TinyLog::LogBuffer

#endif /* __TINYLOG_INCLUDE_BUFFER_H_ */
