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

#include "include/noncopyable.h"
#include <memory>
#include <mutex>
#include <string>

namespace TinyLog {


/* 抽象类 */
class FileUtil {
public:
    FileUtil () = default;
    virtual ~FileUtil () = default; 
    /* 纯虚函数 */
    virtual void append (const char* _msg, size_t _len) = 0;
    virtual void flush () = 0;
    virtual void writtenBytes () const = 0;
};

enum FileUtilType {
    MMAPFileUtil = 0,
    NORMALFileUtil
};

class LogFile : noncopyable {
public:
    LogFile (const std::string &_basename, size_t _rollSize, int _flushInterval, FileUtilType _type) ;
    ~LogFile () = default;
    
    void append (const char* _msg, size_t _len);
    void flush () ;
    bool rollFile() ;

    /* 获取日志文件的名称 */
    static std::string getLogFileName (const std::string& _basename, time_t* now) ;

private:
    const std::string basename_;
    const size_t rollSize_;
    const int flushInterval_;

    std::unique_ptr<FileUtil> file_;
};

}


#endif /* __TINYLOG_INCLUDE_LOGFILE_H_ */
