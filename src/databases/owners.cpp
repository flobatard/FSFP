#include "databases/owners.h"

namespace fsfp::db{
    void owner_metadata_to_raw_val(owner_metadata owner_metadata, fsfp::types::raw_val& raw_val){
        if (raw_val.data != NULL)
        {
            free(raw_val.data);
        }
        raw_val.size = sizeof(owner_metadata.max_data_size);
        raw_val.data = malloc(raw_val.size);
        raw_val.data = &owner_metadata.max_data_size;


    }
    
    void raw_val_to_owner_metadata(fsfp::types::raw_val raw_val, owner_metadata& owner_metadata){



    }
}
