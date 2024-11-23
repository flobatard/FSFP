#include "databases/owners.h"
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
        size_t current_offset = 0;
        if (size && ((current_offset + sizeof(ret.max_data_size)) > size)) {return ret;}
        memcpy(&(ret.max_data_size), (raw_value + current_offset), sizeof(ret.max_data_size));
        current_offset = current_offset + sizeof(ret.max_data_size);
        if (size && ((current_offset + sizeof(ret.active)) > size)) {return ret;}
        memcpy(&(ret.active), (raw_value + current_offset), sizeof(ret.active));
        return ret;
    }
}
