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

int mass_put(LMDBWrapper& lmdb, int offset, int count = 1000)
{
    for (int i = offset; i < count + offset; i++)
    {
        lmdb.put("toto_" + std::to_string(i), "tata_" + std::to_string(i));
    }
    for (int i = offset; i < count + offset; i++)
    {
        std::string key = "toto_" + std::to_string(i);
        std::string value = lmdb.get(key);
        if (!(value == "tata_" + std::to_string(i)))
        {
            std::cerr <<  "Error value PUT is not the same: DB= " << value << " test=" << "tata_" << std::to_string(i) << std::endl; 
            throw "Error value is PUT not the same: DB= " + value + " test=" + "tata_" + std::to_string(i);
        }
    }
    return 0;
}

int mass_delete(LMDBWrapper& lmdb, int offset, int count = 1000)
{
    for (int i = offset; i < count + offset; i++)
    {
        lmdb.remove("toto_" + std::to_string(i));
    }
    for (int i = offset; i < count + offset; i++)
    {
        std::string key = "toto_" + std::to_string(i);
        std::string value = lmdb.get(key);
        if (!(value == ""))
        {
            std::cerr <<  "Error value DELETE is not the same: DB= " << value << " test=" << "tata_" << std::to_string(i) << std::endl; 
            throw "Error value DELETE is not the same: DB= " + value + " test=" + "tata_" + std::to_string(i);
        }
    }
    return 0;
}

int mass_get(LMDBWrapper& lmdb, int offset, int count = 1000)
{
    for (int i = offset; i < count + offset; i++)
    {
        std::string key = "toto_" + std::to_string(i);
        std::string value = lmdb.get(key);
        if (!(value == "tata_" + std::to_string(i)))
        {
            std::cerr <<  "Error value GET is not the same: DB(" << key << ")=" << value << " test=" << "tata_" << std::to_string(i) << std::endl; 
            throw "Error value GET is not the same: DB(toto_" + std::to_string(i) + ")= " + value + " test=" + "tata_" + std::to_string(i);
        }
    }
    return 0;
}

int main()
{
    std::cout << "__cplusplus: " << __cplusplus;
    std::cout << std::endl;
    LMDBWrapper lmdb = LMDBWrapper("db1");
    int nb_chunks = 1000;
    int chunk_size = 100;
    std::vector<std::thread> workers = std::vector<std::thread>(nb_chunks);
    

    auto start = high_resolution_clock::now();

    for (int i = 0; i < nb_chunks; i++)
    {
        workers[i] = std::thread([i, &lmdb, chunk_size] (){
            mass_put(lmdb, i*chunk_size, chunk_size);
        });
    }

    for (int i = 0; i<nb_chunks; i++)
    {
        if (workers[i].joinable())
        {
            workers[i].join();
        }
    }
    auto putClock = high_resolution_clock::now();
    
    auto duration = duration_cast<microseconds>(putClock - start);
    std::cout << "Put chrono " << duration.count() << "micros" << endl;

    for (int i = 0; i < nb_chunks; i++)
    {
         workers[i] = std::thread([i, &lmdb, chunk_size] (){
             mass_get(lmdb, i*chunk_size, chunk_size);
        });
       
    }

    for (int i = 0; i<nb_chunks; i++)
    {
        if (workers[i].joinable())
        {
            workers[i].join();
        }
    }
    auto getClock = high_resolution_clock::now();
    
    auto duration3 = duration_cast<microseconds>(getClock - putClock);
    std::cout << "Get chrono " << duration3.count() << "micros" << endl;
    

    for (int i = 0; i<nb_chunks; i++)
    {
        workers[i] = std::thread([i, &lmdb, chunk_size](){
            mass_delete(lmdb, i*chunk_size, chunk_size);
        });;
    }

    for (int i = 0; i<nb_chunks; i++)
    {
        if (workers[i].joinable())
        {
            workers[i].join();
        }
    }

    auto endClock = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(endClock - putClock);
    std::cout << "Delete chrono " << duration2.count() << "micros" << endl;
    return 0;
}