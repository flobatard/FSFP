#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include "LMDB_wrapper.h"
#include "http_server.h"
#include "databases/registry.h"
#include "databases/owners.h"

#ifdef _WIN32
    #define SEP "\\"
#else
    #define SEP "/"
#endif

using namespace std::chrono;
using namespace std;
using namespace fsfp::db;

int main()
{
    owner_metadata owner_m;
    owner_m.active = 2;
    owner_m.max_data_size = 10;
    cout << "Sizes: " << "u_int:" << sizeof(owner_m.active) << " u_long:" << sizeof(owner_m.max_data_size) << endl;
    cout << "Sizes: " << "void* " << sizeof(void*) << "u_int8_t:" << sizeof(u_int8_t) << " char: " << sizeof(char) << endl;
    cout << "Before: " << owner_m.active << " " << owner_m.max_data_size << endl;
    void* serialized = serialize_owner_metadata(owner_m);
    std::string ser_as_str((char*) serialized, size_of_owner_metadata(owner_m));
    cout << "During: " << ser_as_str << endl;
    owner_metadata after = deserialize_owner_metadata(serialized, sizeof(owner_m.max_data_size));
    cout << "After: " << after.active << " " << after.max_data_size << endl;
    
    

    LMDBWrapper root_lmdb = LMDBWrapper("root");
    DatabasesRegistry* registry = DatabasesRegistry::GetInstance();
    std::cout << "Boost version: " 
          << BOOST_VERSION / 100000
          << "."
          << BOOST_VERSION / 100 % 1000
          << "."
          << BOOST_VERSION % 100 
          << std::endl << BOOST_VERSION << endl << (CROW_USE_BOOST && BOOST_VERSION >= 107000) << endl;
    run_server();
    DatabasesRegistry::DestroyInstance();
    free(serialized);
    return 0;
}