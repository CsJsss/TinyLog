#include "leveldb/db.h"
#include "leveldb/options.h"
#include <iostream>

using namespace std;

int main(int, char **) {
    leveldb::Options opt;
    string logName = "main.cpp-LeveldbLog-20220530-101638.639-2.1655516.log";
    leveldb::DB* db;
    auto status = leveldb::DB::Open(opt, logName, &db);

    if (!status.ok()) {
        cout << "open leveldb log file error\n";
        return 0;
    }
    leveldb::Iterator *it = db->NewIterator(leveldb::ReadOptions());
    size_t cnt = 0;
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        cout << it->key().ToString() << it->value().ToString();
        ++ cnt;
    }
    cout << endl << "items = " << cnt << endl;
    return 0;
}
