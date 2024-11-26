#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include "LMDB_wrapper.h"
#include "http_server.h"
#include "databases/registry.h"
#include "databases/owners.h"
#include "databases/files.h"

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
    DatabasesRegistry::GetInstance();
    std::cout << "Boost version: " 
          << BOOST_VERSION / 100000
          << "."
          << BOOST_VERSION / 100 % 1000
          << "."
          << BOOST_VERSION % 100 
          << std::endl << BOOST_VERSION << endl << (CROW_USE_BOOST && BOOST_VERSION >= 107000) << endl;
    run_server();
    DatabasesRegistry::DestroyInstance();
    return 0;
}