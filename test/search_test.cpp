#include<iostream>
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
        lmdb.put("toto_" + std::to_string(i), LMDBWrapper::string_to_mdb_val("tata_" + std::to_string(i)));
    }
    for (int i = offset; i < count + offset; i++)
    {
        std::string value = LMDBWrapper::mdb_val_to_string(lmdb.get("toto_" + std::to_string(i)));
        if (!(value == "tata_" + std::to_string(i)))
        {
            std::cerr << "Error value is not the same" << std::endl; 
            throw "Error value is not the same: DB= " + value + " test=" + "tata_" + std::to_string(i);
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
        std::string value = LMDBWrapper::mdb_val_to_string(lmdb.get("toto_" + std::to_string(i)));
        if (!(value == ""))
        {
            std::cerr << "Error value is not the same" << std::endl; 
            throw "Error value is not the same: DB= " + value + " test=" + "tata_" + std::to_string(i);
        }
    }
    return 0;
}

int mass_get(LMDBWrapper& lmdb, int offset, int count = 1000)
{
    for (int i = offset; i < count + offset; i++)
    {
        std::string value = LMDBWrapper::mdb_val_to_string(lmdb.get("toto_" + std::to_string(i)));
        if (!(value == "tata_" + std::to_string(i)))
        {
            std::cerr << "Error value is not the same" << std::endl;
            throw "Error value is not the same: DB= " + value + " test=" + "tata_" + std::to_string(i);
        }
    }
    return 0;
}

int main()
{
    std::cout << "__cplusplus: " << __cplusplus;
    std::cout << std::endl;
    LMDBWrapper lmdb = LMDBWrapper("db1");
    int nb_chunks = 10;
    int chunk_size = 10;

    auto start = high_resolution_clock::now();

    for (int i = 0; i < nb_chunks; i++)
    {
        mass_put(lmdb, i*chunk_size, chunk_size);
    }
    auto putClock = high_resolution_clock::now();
    
    auto duration = duration_cast<microseconds>(putClock - start);
    std::cout << "Put chrono " << duration.count() << "micros" << endl;

    for (int i = 0; i < nb_chunks; i++)
    {
        mass_get(lmdb, i*chunk_size, chunk_size);
    }
    auto getClock = high_resolution_clock::now();

    auto duration3 = duration_cast<microseconds>(getClock - putClock);
    std::cout << "Get chrono " << duration3.count() << "micros" << endl;

    auto pKeysClock = high_resolution_clock::now();
    std::vector<std::string> vec = lmdb.keys();
    auto keysClock = high_resolution_clock::now();
    auto durationKeys = duration_cast<microseconds>(keysClock - pKeysClock);
    std::cout << "Keys chrono " << durationKeys.count() << "micros" << endl;


    std::cout << "Done " << duration3.count() << "micros" << endl;
    std::cout << "Count: " << vec.size() << endl;
    for (unsigned int i = 0; i < vec.size(); i++)
    {
        std::cout << vec[i] << " ";
    }
    std::cout << endl << endl;

    auto pregetStartClock = high_resolution_clock::now();
    std::vector<std::string> vec2 = lmdb.search_keys_starts_with("toto_1");
    auto getStartClock = high_resolution_clock::now();
    auto durationGetStart = duration_cast<microseconds>(getStartClock - pregetStartClock);
    std::cout << "StartWith chrono " << durationGetStart.count() << "micros" << endl;
    for (unsigned int i = 0; i < vec2.size(); i++)
    {
        std::cout << vec2[i] << " ";
    }
    std::cout << endl;

    auto predeleteStartClock = high_resolution_clock::now();
    int count = lmdb.delete_starts_with("toto_1");
    auto getDeleteClock = high_resolution_clock::now();
    auto durationDeleteStart = duration_cast<microseconds>(getDeleteClock - predeleteStartClock);
    std::cout << "DeleteWith chrono " << durationDeleteStart.count() << "micros" << endl;
    std::cout << "Delete with count " << count << endl;
    std::vector<std::string> vec3 = lmdb.keys();
    for (unsigned int i = 0; i < vec3.size(); i++)
    {
        std::cout << vec3[i] << " ";
    }
    std::cout << endl;
    
    for (int i = 0; i<nb_chunks; i++)
    {
        mass_delete(lmdb, i*chunk_size, chunk_size);
    }

    auto endClock = high_resolution_clock::now();
    auto duration2 = duration_cast<microseconds>(endClock - putClock);
    std::cout << "Delete chrono " << duration2.count() << "micros" << endl;
    return 0;
}