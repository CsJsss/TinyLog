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
#include <unistd.h>

#ifndef __TINYLOG_INCLUDE_THREADINFO_H_
#define __TINYLOG_INCLUDE_THREADINFO_H_

namespace TinyLog ::ThreadInfo {

extern thread_local int currentTid;
extern thread_local char tidStr[32];
extern thread_local size_t tidStrlen;

/* 线程ID信息 */
int getTid();
const char *getTidStr();
size_t getTidStrlen();

} // namespace TinyLog::ThreadInfo

#endif /* __TINYLOG_INCLUDE_THREADINFO_H_ */
