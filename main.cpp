#include<iostream>
#include<thread>
#include<vector>
#include<chrono>
#include"LMDB_wrapper.h"

#ifdef _WIN32
    #define SEP "\\"
#else
    #define SEP "/"
#endif

using namespace std::chrono;
using namespace std;

int main()
{
    LMDBWrapper lmdb = LMDBWrapper("db1");
    return 0;
}