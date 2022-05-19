/**
 * @file fileWriter.cpp
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-05-03
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "include/FileWriter.h"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>

namespace TinyLog {

/* 构造函数根据文件名新建位于当前路径的文件 */
NormalFileWriter::NormalFileWriter(const std::string &_fileName)
    { /*FILE mode : we */
  // printf("path = %s\n", _fileName.c_str());
  fp_ = ::fopen(_fileName.c_str(), "w");
  if (fp_ == nullptr) {
    perror("NormalFileWriter open file failed: ");
    int err = ferror(fp_);
    fprintf(stderr, "NormalFileWriter open file: %s failed, errno: %s\n",
            _fileName.c_str(), strerror(err));
    abort();
  }
  ::setbuffer(fp_, buffer_, sizeof(buffer_));
}

/* 析构函数中先刷盘, 然后关闭文件描述符 */
NormalFileWriter::~NormalFileWriter() {
  flush();
  if (fp_ != nullptr)
    ::fclose(fp_);
}

void NormalFileWriter::append(const char *_msg, size_t _len) {
  /* muduo */
  size_t written = 0;

  while (written != _len) {
    size_t remain = _len - written;
    size_t n = ::fwrite_unlocked(_msg + written, 1, remain, fp_);
    if (n != remain) {
      int err = ferror(fp_);
      if (err) {
        fprintf(stderr, "NormalFileWriter::append failed, errno: %s\n",
                strerror(err));
        break;
      }
    }
    written += n;
  }

  writen_ += written;
}

void NormalFileWriter::flush() { fflush(fp_); }
size_t NormalFileWriter::writtenBytes() const { return writen_; }

} // namespace TinyLog