#ifndef LMDB_WRAPPER_H
#define LMDB_WRAPPER_H

#include <string>
#include <mutex>
#include <tuple>
#include <vector>
#include "lmdb.h"

class LMDBWrapper
{
    public:
        LMDBWrapper(const std::string& dirPath);
        ~LMDBWrapper();
        std::string get(const std::string& key);
        int put(const std::string& key, const std::string& value);
        int remove(const std::string& key);
        std::vector<std::string> keys();
        std::vector<std::string> search_starts_with(const std::string& starts);
        int delete_starts_with(const std::string& starts);
        std::tuple<MDB_dbi, MDB_txn*> begin_transaction(bool read_only = false);
        int end_transaction(MDB_txn* mdb_transaction);
        int abort(MDB_txn* mdb_transaction);

        std::mutex dbi_mtx;
        MDB_env* mdb_env;
        MDB_cursor* mdb_cursor;

};

#endif //LMDB_WRAPPER_H