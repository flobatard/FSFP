#ifndef DATABASES_OWNERS_H
#define DATABASES_OWNERS_H

#include "fsfp_types.h"
#include <cstddef>
#include <string>
#include "LMDB_wrapper.h"

namespace fsfp::db{
    struct owner_metadata{
        unsigned long max_data_size;
        unsigned int active;
    };

    size_t size_of_owner_metadata(const owner_metadata& owner_metadata);
    uint8_t* serialize_owner_metadata(owner_metadata& owner_metadata);
    owner_metadata deserialize_owner_metadata(uint8_t* raw_value, size_t size = 0);

    int owner_put(LMDBWrapper* lmdb, std::string owner, owner_metadata& owner_m);
    int owner_get(LMDBWrapper* lmdb, std::string owner, owner_metadata& owner_m);
    int owner_del(LMDBWrapper* lmdb, std::string owner);
}

#endif //DATABASES_OWNERS_H