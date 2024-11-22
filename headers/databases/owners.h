#include "fsfp_types.h"
#include <cstddef>
#include <string>
#include "LMDB_wrapper.h"

namespace fsfp::db{
    struct owner_metadata{
        unsigned long max_data_size;
    };

    void owner_metadata_to_raw_val(owner_metadata owner_metadata, fsfp::types::raw_val& raw_val);
    void raw_val_to_owner_metadata(fsfp::types::raw_val raw_val, owner_metadata& owner_metadata);

    int owner_put(LMDBWrapper* lmdb, std::string owner, owner_metadata& owner_metadata);
    owner_metadata owner_get(LMDBWrapper* lmdb, std::string owner);
    int owner_del(LMDBWrapper* lmdb, std::string owner);
}