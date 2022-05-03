/**
 * @file threadInfo.h
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-05-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <cstddef>
#include <cstdio>
#include <string>
#include <unistd.h>

#ifndef __TINYLOG_INCLUDE_THREADINFO_H_
#define __TINYLOG_INCLUDE_THREADINFO_H_

namespace TinyLog ::ThreadInfo {

/* 线程信息缓存在TLS中 */
extern thread_local int currentTid;
extern thread_local char tidStr[32];
extern thread_local size_t tidStrlen;

/* 线程ID信息 */
int getTid();
const char *getTidStr();
size_t getTidStrlen();


/* 进程ID信息 */
pid_t getPid ();
std::string getPidStr();

} // namespace TinyLog::ThreadInfo

#endif /* __TINYLOG_INCLUDE_THREADINFO_H_ */
