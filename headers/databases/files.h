#ifndef DATABASES_FILES_H
#define DATABASES_FILES_H


#include "fsfp_types.h"
#include <cstddef>
#include <string>
#include "LMDB_wrapper.h"

namespace fsfp::db{

    struct file_metadata {
        unsigned long file_size;
        unsigned int scope;
        std::string key;
    };

    size_t size_of_file_metadata(const file_metadata& file_m);
    uint8_t* serialize_file_metadata(file_metadata& file_m);
    file_metadata deserialize_file_metadata(uint8_t* raw_value, size_t size = 0);

    int file_put(LMDBWrapper* lmdb, std::string owner, file_metadata& file_m);
    file_metadata file_get(LMDBWrapper* lmdb, std::string owner);
    int file_del(LMDBWrapper* lmdb, std::string owner);
}

#endif //DATABASES_FILES_H