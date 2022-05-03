/**
 * @file fileUtil.h
 * @author Jsss (764527108@qq.com)
 * @brief 
 * @version 0.1
 * @date 2022-05-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __TINYLOG_INCLUDE_FILEWRITER_H_
#define __TINYLOG_INCLUDE_FILEWRITER_H_
#include "include/noncopyable.h"
#include <cstddef>
#include <string_view>

/* 文件的抽象类, 实现类需要实现具体的文件读写方式 */
class FileWriter : noncopyable{
public:
    FileWriter () = default;
    virtual ~FileWriter () = default; 
    /* 纯虚函数 */
    virtual void append (const char* _msg, size_t _len) = 0;
    virtual void flush () = 0;
    virtual void writtenBytes () const = 0;
};


/* 目前实现的 FileUtilType */
enum FileWriterType {
    MMAPFileWriter = 0,
    NORMALFileWriter
};


class MmapFileWriter : public FileWriter {
public:
    explicit MmapFileWriter (std::string_view _fileName) ;
    ~MmapFileWriter() override;

    /* 重写 */
    void append (const char* _msg, size_t _len) override;
    void flush () override;
    void writtenBytes () const override; 
};


class NormalFileWriter : public FileWriter {
public:
    explicit NormalFileWriter (std::string_view _fileName);
    ~NormalFileWriter() override;

    /* 重写 */
    void append (const char* _msg, size_t _len) override;
    void flush () override;
    void writtenBytes () const override; 
};

#endif /* __TINYLOG_INCLUDE_FILEWRITER_H_ */
