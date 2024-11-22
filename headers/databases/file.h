#include "fsfp_types.h"
#include <cstddef>
#include <string>
#include "LMDB_wrapper.h"

namespace fsfp::db{

    struct file_metadata {
        unsigned long file_size;
        unsigned int scope;
        char* key;
        size_t key_size;
    };

    fsfp::types::raw_val file_metadata_to_raw_val(file_metadata file_metadata);
    file_metadata raw_val_to_file_metadata(fsfp::types::raw_val file_metadata);

    int file_put(LMDBWrapper* lmdb, std::string owner, file_metadata& owner_metadata);
    file_metadata file_get(LMDBWrapper* lmdb, std::string owner);
    int file_del(LMDBWrapper* lmdb, std::string owner);
}