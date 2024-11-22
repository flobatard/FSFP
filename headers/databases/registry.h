#ifndef DATABASES_REGISTRY_H
#define DATABASES_REGISTRY_H

#include "LMDB_wrapper.h"
#include <string>
#include <mutex>
#include <tuple>
#include <map>

class DatabasesRegistry
{
    public:
        static DatabasesRegistry* instance;
        static std::mutex mtx;
        DatabasesRegistry(DatabasesRegistry &other) = delete;
        void operator=(const DatabasesRegistry &) = delete;
        static DatabasesRegistry* GetInstance();
        static void DestroyInstance();
        LMDBWrapper* getOwnerDatabase(const std::string& owner);
        LMDBWrapper* getRootDatabase();
        LMDBWrapper* getRegistryDatabase();
        int insertNewOwner(std::string owner, LMDBWrapper* lmdb);
        DatabasesRegistry();
        ~DatabasesRegistry();
        

    protected:
        std::map<std::string, LMDBWrapper*> registry;
        LMDBWrapper* root_db;
        LMDBWrapper* registry_db;
};

#endif //DATABASES_REGISTRY_H