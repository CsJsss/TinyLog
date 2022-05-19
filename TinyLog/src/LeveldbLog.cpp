#include "include/LeveldbLog.h"
#include "leveldb/options.h"
#include "leveldb/write_batch.h"
#include <cassert>

namespace TinyLog {

LeveldbLog::LeveldbLog(const std::string _basename, int _flushInterval)
    : basename_(_basename), flushInterval_(_flushInterval) {
  prevSecond = static_cast<time_t>(0);
  leveldb::Options opt;
  opt.create_if_missing = true;
  auto status =
      leveldb::DB::Open(opt, LogFile::getLogFileName(_basename), &db_);
  if (!status.ok()) {
    fprintf(stderr, "LeveldbLog Error: %s\n", status.ToString().c_str());
    abort();
  }
}

LeveldbLog::~LeveldbLog() {
  if (db_ != nullptr) {
    flush();
    delete db_;
  }
}

/* 前端和后端的唯一接口 */
void LeveldbLog::append(const char *_msg, size_t _len, size_t _keyLen) {
  assert(_len >= _keyLen);

  std::unique_lock<std::mutex> lock(mtx_);
  leveldb::Slice key(_msg, _keyLen), value(_msg + _keyLen, _len - _keyLen);
  batch.Put(key, value);
  time_t now = Timestamp::now().getSeconds();
  if (static_cast<time_t>(now - prevSecond) >= flushInterval_) {
    flush();
    prevSecond = now;
  }
}

void LeveldbLog::flush() {
  auto status = db_->Write(leveldb::WriteOptions(), &batch);
  if (!status.ok()) {
    fprintf(stderr, "LeveldbLog flush Error: %s\n", status.ToString().c_str());
  }
  batch.Clear();
}

} // namespace TinyLog