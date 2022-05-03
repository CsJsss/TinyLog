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
#include "include/threadInfo.h"
#include <unistd.h>

namespace TinyLog ::ThreadInfo {

thread_local int currentTid = 0;
thread_local char tidStr[32];
thread_local size_t tidStrlen = 0;

int getTid() {
  if (currentTid == 0) {
    currentTid = ::gettid();
    tidStrlen = snprintf(tidStr, sizeof(tidStr), "%6d ", currentTid);
  }
  return currentTid;
}

const char *getTidStr() { return tidStr; }

size_t getTidStrlen() { return tidStrlen; }

} // namespace TinyLog::ThreadInfo