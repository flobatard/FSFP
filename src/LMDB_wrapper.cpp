#include"LMDB_wrapper.h"
#include<iostream>

#include<lmdb.h>
#include <string>
#include <filesystem>
#include <tuple>


std::string sep = 
    #ifdef _WIN32
        "\\";
    #else
        "/";
    #endif

std::string baseDir = "data";

using namespace std;

tuple<MDB_dbi, MDB_txn*> LMDBWrapper::begin_transaction(bool read_only)
{
    
    MDB_txn* mdb_transaction;
    if(const int rc = mdb_txn_begin(mdb_env, NULL, (read_only ?  MDB_RDONLY : 0), &mdb_transaction)){
        throw runtime_error("Couldn't start LMDB txn mdb_txn_begin() returned " + string(mdb_strerror(rc)));
    }

    dbi_mtx.lock();
    MDB_dbi mdb_dbi;
    if(const int rc = mdb_dbi_open(mdb_transaction, 0, MDB_CREATE, &mdb_dbi))
    {
        throw runtime_error("Couldn't open LMDB zone database  mdb_dbi_open() returned " + string(mdb_strerror(rc)));
    }
    dbi_mtx.unlock();
   
    tuple<MDB_dbi, MDB_txn*> ret = make_tuple(mdb_dbi, mdb_transaction);
    return ret;
}

int LMDBWrapper::end_transaction(MDB_txn* mdb_transaction)
{
    const int rc = mdb_txn_commit(mdb_transaction);
    return rc;
}

int LMDBWrapper::abort(MDB_txn* mdb_transaction)
{
    mdb_txn_abort(mdb_transaction);
    return 0;
}

// Méthode pour insérer une paire clé-valeur
int LMDBWrapper::put(const std::string& key, MDB_val value) {
    MDB_dbi mdb_dbi;
    MDB_txn* mdb_transaction;
    std::tie(mdb_dbi, mdb_transaction) = begin_transaction();

    MDB_val mdb_key;
    mdb_key.mv_size = key.size();
    mdb_key.mv_data = (void*)key.c_str();
    if (const int rc = mdb_put(mdb_transaction, mdb_dbi, &mdb_key, &value, 0))
    {
        std::cerr << "Error putting data: " << rc << ":" << mdb_strerror(rc) << std::endl;
        abort(mdb_transaction);
        throw runtime_error("Couldn't put LMDB zone database  mdb_put() returned " + string(mdb_strerror(rc)));
    }
    end_transaction(mdb_transaction);
    return 0;
}

// Méthode pour récupérer la valeur associée à une clé
MDB_val LMDBWrapper::get(const std::string& key) {
    MDB_dbi mdb_dbi;
    MDB_txn* mdb_transaction;
    std::tie(mdb_dbi, mdb_transaction) = begin_transaction(true);

    MDB_val mdb_key, mdb_value;
    mdb_key.mv_size = key.size();
    mdb_key.mv_data = (void*)key.c_str();
    mdb_value.mv_size = 0;
    mdb_value.mv_data = NULL;

    int rc = mdb_get(mdb_transaction, mdb_dbi, &mdb_key, &mdb_value);
    if (rc == MDB_NOTFOUND) {
        abort(mdb_transaction);
        return mdb_value;  // Clé non trouvée
    } else if (rc != 0) {
        abort(mdb_transaction);
        throw runtime_error("Couldn't GET LMDB zone database  mdb_get() returned " + string(mdb_strerror(rc)));
    }
    end_transaction(mdb_transaction);
    return mdb_value;
}

// Méthode pour supprimer une paire clé-valeur
int LMDBWrapper::remove(const std::string& key) {
    MDB_dbi mdb_dbi;
    MDB_txn* mdb_transaction;
    std::tie(mdb_dbi, mdb_transaction) = begin_transaction();

    MDB_val mdb_key;
    mdb_key.mv_size = key.size();
    mdb_key.mv_data = (void*)key.c_str();


    int rc = mdb_del(mdb_transaction, mdb_dbi, &mdb_key, NULL);
    if (rc == MDB_NOTFOUND) {
        abort(mdb_transaction);
        return rc;
    } else if (rc != 0) {
        abort(mdb_transaction);
        throw runtime_error("Couldn't DELETE LMDB zone database  mdb_del() returned " + string(mdb_strerror(rc)));
        return rc;
    }

    end_transaction(mdb_transaction);
    return 0;
}

std::vector<std::string> LMDBWrapper::keys() {
    MDB_dbi mdb_dbi;
    MDB_txn* mdb_transaction;
    MDB_cursor* mdb_cursor;
    std::vector<std::string> ret = {};
    std::tie(mdb_dbi, mdb_transaction) = begin_transaction(true);
    int rc = mdb_cursor_open(mdb_transaction, mdb_dbi, &mdb_cursor);
    if (rc != 0) {
        mdb_cursor_close(mdb_cursor);
        end_transaction(mdb_transaction);
        return ret;
    }


    MDB_val key, value;
    std::string key_retrieved;
    while (mdb_cursor_get(mdb_cursor, &key, &value, MDB_NEXT) == 0) {
        key_retrieved.assign((char *)key.mv_data, key.mv_size);
        ret.push_back(key_retrieved);
    }

    mdb_cursor_close(mdb_cursor);
    end_transaction(mdb_transaction);
    return ret;
}

std::vector<std::string> LMDBWrapper::search_keys_starts_with(const std::string& starts) {
    MDB_dbi mdb_dbi;
    MDB_txn* mdb_transaction;
    MDB_cursor* mdb_cursor;
    std::vector<std::string> ret = {};
    std::tie(mdb_dbi, mdb_transaction) = begin_transaction(true);
    int rc = mdb_cursor_open(mdb_transaction, mdb_dbi, &mdb_cursor);
    if (rc != 0) {
        cerr << "Unable to open cursor" << endl;
        mdb_cursor_close(mdb_cursor);
        end_transaction(mdb_transaction);
        return ret;
    }

    MDB_val key;
    key.mv_size = starts.size();
    key.mv_data = (void*)starts.c_str();
    MDB_val mdb_value;

    rc = mdb_cursor_get(mdb_cursor, &key, &mdb_value, MDB_SET_RANGE);
    if (rc != 0){
        cout << "UNABLE TO GET WITH CURSOR: "  << rc << " : " << string(mdb_strerror(rc)) << endl;
        return ret;
    }

    std::string value;
    while (rc == 0)
    {
        value.assign((char*) key.mv_data, key.mv_size);
        if (value.rfind(starts, 0) != 0)
        {
            mdb_cursor_close(mdb_cursor);
            end_transaction(mdb_transaction);    
            return ret;
        }

        ret.push_back(value);
        rc = mdb_cursor_get(mdb_cursor, &key, &mdb_value, MDB_NEXT);
    }

    mdb_cursor_close(mdb_cursor);
    end_transaction(mdb_transaction);    
    return ret;
}

int LMDBWrapper::delete_starts_with(const std::string& starts) {
    MDB_dbi mdb_dbi;
    MDB_txn* mdb_transaction;
    MDB_cursor* mdb_cursor;
    std::tie(mdb_dbi, mdb_transaction) = begin_transaction();
    int rc = mdb_cursor_open(mdb_transaction, mdb_dbi, &mdb_cursor);
    int ret = 0;

    

    if (rc != 0) {
        cerr << "Unable to open cursor" << endl;
        mdb_cursor_close(mdb_cursor);
        end_transaction(mdb_transaction);
        return ret;
    }

    MDB_val key;
    key.mv_size = starts.size();
    key.mv_data = (void*)starts.c_str();
    MDB_val mdb_value;

    rc = mdb_cursor_get(mdb_cursor, &key, &mdb_value, MDB_SET_RANGE);
    if (rc != 0){
        cout << "UNABLE TO GET WITH CURSOR: "  << rc << " : " << string(mdb_strerror(rc)) << endl;
        return ret;
    }

    std::string value;
    while (rc == 0)
    {
        value.assign((char*) key.mv_data, key.mv_size);
        if (value.rfind(starts, 0) != 0)
        {
            mdb_cursor_close(mdb_cursor);
            end_transaction(mdb_transaction);    
            return ret;
        }
        ret = ret + 1;
        rc = mdb_cursor_del(mdb_cursor, 0);
        if (rc != 0)
        {
            cerr << "Error while deleting" << rc << " : " << string(mdb_strerror(rc)) << endl;
            mdb_cursor_close(mdb_cursor);
            end_transaction(mdb_transaction);
            return ret;
        }
        rc = mdb_cursor_get(mdb_cursor, &key, &mdb_value, MDB_NEXT);
    }

    mdb_cursor_close(mdb_cursor);
    end_transaction(mdb_transaction);
    return 0;
}

LMDBWrapper::LMDBWrapper(const string& dirPath)
{
    try{
        if(std::filesystem::create_directory(baseDir + sep + dirPath))
        {
            std::cout << "Created a directory " << dirPath << endl;
        }
    }catch(const std::exception& e){
        std::cerr << "Error on directory creation: " << dirPath << " " << e.what() << endl;
    }
    std::cout << "Setting up Database env" << endl;
    if (const int rc = mdb_env_create(&mdb_env))
    {
        std::cerr << "Error creating env" << mdb_strerror(rc) << endl;
    }
    std::cout << "MDB ENV Created" << endl;
    if (const int rc = mdb_env_set_maxdbs(mdb_env, 50))
    {
        std::cerr << "Error Max DBS: " << mdb_strerror(rc) << endl;
    }
    if (const int rc = mdb_env_set_mapsize(mdb_env, (size_t)1048576 * (size_t)100000))// 1MB * 100000
    {
        std::cerr << "Error Mapsize: " <<  mdb_strerror(rc) << endl;
    }; 
    if (const int rc = mdb_env_open(mdb_env, (baseDir + sep + dirPath).c_str(), 0, 0664))
    {
        std::cerr << "Error creating env: " << mdb_strerror(rc) << endl;
    }
    std::cout << "Database launched " << dirPath << endl;
}

std::string LMDBWrapper::mdb_val_to_string(MDB_val val){
    if (val.mv_size == 0)
    {
        return "";
    }
    std::string ret((char*)val.mv_data, val.mv_size);
    return ret;
}
MDB_val LMDBWrapper::string_to_mdb_val(std::string val){
    MDB_val ret;
    ret.mv_data = (void*)val.c_str();
    ret.mv_size = val.size();
    return ret;
}

LMDBWrapper::~LMDBWrapper()
{
    mdb_env_close(mdb_env);
    cout << "Destroyed" <<endl;
}