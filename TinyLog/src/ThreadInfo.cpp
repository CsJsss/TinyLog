/**
 * @file threadInfo.cpp
 * @author Jsss (764527108@qq.com)
 * @brief
 * @version 0.1
 * @date 2022-05-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "include/ThreadInfo.h"
#include <unistd.h>

namespace TinyLog ::ThreadInfo {

thread_local int currentTid = 0;
thread_local char tidStr[32];
thread_local size_t tidStrlen = 0;

int getTid() {
  if (currentTid == 0) {
    currentTid = ::gettid();
    tidStrlen = static_cast<size_t>(snprintf(tidStr, sizeof(tidStr), "%d ", currentTid));
  }
  return currentTid;
}

const char *getTidStr() { return tidStr; }

size_t getTidStrlen() { return tidStrlen; }

pid_t getPid() { return ::getpid(); }

std::string getPidStr() {
  return std::to_string(getPid());
  // char buff[32];
  // snprintf(buff, sizeof(buff), "%d", getPid());
  // return buff;
}

} // namespace TinyLog::ThreadInfo