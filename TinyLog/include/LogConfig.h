/**
 * @file logConfig.h
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-05-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __TINYLOG_INCLUDE_LOGCONFIG_H_
#define __TINYLOG_INCLUDE_LOGCONFIG_H_

#include "include/FileWriter.h"
#include "include/Logging.h"

namespace TinyLog {
class LogConfig {
public:
  /* 默认日志等级 */
  Logger::LogLevel logLevel = Logger::INFO;

  struct FileOptions {
    std::string baseName = "must_can't_be_null";
    /* 日志滚动大小和刷盘间隔, 用于AsynLog */
    size_t rooSize = 1024 * 1024 ;
    int flushInterval = 3;
    /* 默认buffer个数 */
    int bufferNums = 4;
    int maxBuffToWrite = 16;

    /* 写文件的具体方式 */
    FileWriterType fileWriter = NORMALFileWriter;
  };

  FileOptions fileOption;
};
} // namespace TinyLog

#endif /* __TINYLOG_INCLUDE_LOGCONFIG_H_ */
