/**
 * @file LogFile.h
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-05-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __TINYLOG_INCLUDE_LOGFILE_H_
#define __TINYLOG_INCLUDE_LOGFILE_H_

#include "include/FileWriter.h"
#include "include/noncopyable.h"
#include <memory>
#include <mutex>
#include <string>

namespace TinyLog {

class LogFile : noncopyable {
public:
  LogFile(const std::string &_basename, size_t _rollSize,
          FileWriterType _fileWriterType);
  ~LogFile() = default;

  void append(const char *_msg, size_t _len);
  void flush();
  void rollFile();

  /* 获取日志文件的名称 */
  static std::string getLogFileName(const std::string &_basename);

private:
  const std::string basename_;
  const size_t rollSize_;
  FileWriterType fileWriterType_;

  /* File的独占指针 */
  std::unique_ptr<FileWriter> file_;
};

} // namespace TinyLog

#endif /* __TINYLOG_INCLUDE_LOGFILE_H_ */
