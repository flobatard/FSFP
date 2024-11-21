#include<iostream>
#include<thread>
#include<vector>
#include<chrono>
#include"LMDB_wrapper.h"
#include"http_server.h"

#ifdef _WIN32
    #define SEP "\\"
#else
    #define SEP "/"
#endif

using namespace std::chrono;
using namespace std;

int main()
{
    LMDBWrapper root_lmdb = LMDBWrapper("root");
    std::cout << "Boost version: " 
          << BOOST_VERSION / 100000
          << "."
          << BOOST_VERSION / 100 % 1000
          << "."
          << BOOST_VERSION % 100 
          << std::endl << BOOST_VERSION << endl << (CROW_USE_BOOST && BOOST_VERSION >= 107000) << endl;
    run_server();
    return 0;
}