#include "databases/owners.h"
#include "LMDB_wrapper.h"
#include <cstring> 

using namespace fsfp::types;

namespace fsfp::db{
    size_t size_of_owner_metadata(const owner_metadata& owner_m){
        return sizeof(owner_m.max_data_size) + sizeof(owner_m.active);
    }

    uint8_t* serialize_owner_metadata(owner_metadata& owner_m){
        uint8_t* ret = (uint8_t*)malloc(size_of_owner_metadata(owner_m));
        size_t current_offset = 0;
        memcpy(ret + current_offset, &(owner_m.max_data_size), sizeof(owner_m.max_data_size));
        current_offset = current_offset + sizeof(owner_m.max_data_size);
        memcpy(ret + current_offset, &(owner_m.active), sizeof(owner_m.active));
        return ret;
    }

    owner_metadata deserialize_owner_metadata(uint8_t* raw_value, size_t size){
        owner_metadata ret;
        ret.max_data_size = 0;
        size_t current_offset = 0;
        if (size && ((current_offset + sizeof(ret.max_data_size)) > size)) {return ret;}
        memcpy(&(ret.max_data_size), (raw_value + current_offset), sizeof(ret.max_data_size));
        current_offset = current_offset + sizeof(ret.max_data_size);
        if (size && ((current_offset + sizeof(ret.active)) > size)) {return ret;}
        memcpy(&(ret.active), (raw_value + current_offset), sizeof(ret.active));
        return ret;
    }

    int owner_put(LMDBWrapper* lmdb, std::string owner, owner_metadata& owner_metadata){
        uint8_t* raw_val = serialize_owner_metadata(owner_metadata);
        size_t val_size = size_of_owner_metadata(owner_metadata);
        MDB_val val;
        val.mv_data = raw_val;
        val.mv_size = val_size;
        int ret = lmdb->put(owner, val);
        free(raw_val);
        return ret;
    }

    int owner_get(LMDBWrapper* lmdb, std::string owner, owner_metadata& ret){
        MDB_val v = lmdb->get(owner);
        if (v.mv_size == 0) {return 404;}
        ret = deserialize_owner_metadata((uint8_t*)v.mv_data, v.mv_size);
        return 0;
    }
    int owner_del(LMDBWrapper* lmdb, std::string owner){
        return lmdb->remove(owner);
    }
}
