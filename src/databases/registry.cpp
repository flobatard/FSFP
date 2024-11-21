#include <mutex>
#include "databases/registry.h"
#include <filesystem>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

DatabasesRegistry::DatabasesRegistry() {
    fs::path dbs_path("dbs");
    fs::create_directories(dbs_path / "owners");

    cout << "Setup registry db: " << endl;
    registry_db = new LMDBWrapper(dbs_path / "registry");
    cout << "Setup root db: " << endl;
    root_db = new LMDBWrapper(dbs_path / "root");
    
    vector<string> owner_list = registry_db->keys();

    for (unsigned int i = 0; i < owner_list.size(); i++)
    {
        cout << "Owner: " << owner_list[i] << endl;
        registry.insert(std::pair<std::string, LMDBWrapper*>(owner_list[i], new LMDBWrapper(owner_list[i])));
    }

    std::cout << "Registry launched" << endl;
}

DatabasesRegistry::~DatabasesRegistry() {
    cout << "Deleting Registery LMDB: " << endl;
    delete registry_db;
    cout << "Deleting Root LMDB: " << endl;
    delete root_db;

    for (auto it=registry.begin(); it!=registry.end(); ++it)
    {
        cout << "Deleting Owner LMDB: " << it->first << endl;
        delete it->second;
    }
    std::cout << "Registry deleted" << endl;
}

DatabasesRegistry* DatabasesRegistry::GetInstance() {
        // Acquire lock before checking instance
        lock_guard<mutex> lock(DatabasesRegistry::mtx); // automatically releases lock
        if (DatabasesRegistry::instance == nullptr) {
            DatabasesRegistry::instance = new DatabasesRegistry(); // Create the instance only once
        }
        return DatabasesRegistry::instance;
}

void DatabasesRegistry::DestroyInstance() {
    delete DatabasesRegistry::instance;
    instance = NULL;
}

// Initialize static members outside the class definition
mutex DatabasesRegistry::mtx;
DatabasesRegistry* DatabasesRegistry::instance = nullptr;