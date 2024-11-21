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
        MDB_val get(const std::string& key);
        int put(const std::string& key, MDB_val value);
        int remove(const std::string& key);
        std::vector<std::string> keys();
        std::vector<std::string> search_keys_starts_with(const std::string& starts);
        int delete_starts_with(const std::string& starts);
        std::tuple<MDB_dbi, MDB_txn*> begin_transaction(bool read_only = false);
        int end_transaction(MDB_txn* mdb_transaction);
        int abort(MDB_txn* mdb_transaction);

        std::mutex dbi_mtx;
        MDB_env* mdb_env;
        MDB_cursor* mdb_cursor;

        static std::string mdb_val_to_string(MDB_val val);
        static MDB_val string_to_mdb_val(std::string val);
};

#endif //LMDB_WRAPPER_H