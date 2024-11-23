#include "databases/file.h"
#include <cstring>
#include <iostream>

using namespace std;

namespace fsfp::db{
    size_t size_of_file_metadata(const file_metadata& file_m){
        size_t ret = 0;
        ret += sizeof(file_m.file_size);
        ret += sizeof(file_m.scope);
        ret += sizeof(size_t);
        ret += file_m.key.size() + 1;
        return ret;
    }

    uint8_t* serialize_file_metadata(file_metadata& file_m){
        uint8_t* ret = (uint8_t*)malloc(size_of_file_metadata(file_m));
        
        size_t current_offset = 0;
        memcpy(ret + current_offset, &(file_m.file_size), sizeof(file_m.file_size));
        current_offset = current_offset + sizeof(file_m.file_size);

        memcpy(ret + current_offset, &(file_m.scope), sizeof(file_m.scope));
        current_offset = current_offset + sizeof(file_m.scope);

        size_t key_size =file_m.key.size();
        memcpy(ret + current_offset, &(key_size), sizeof(key_size));
        current_offset = current_offset + sizeof(key_size);

        memcpy(ret + current_offset, file_m.key.c_str(), key_size);
        current_offset = current_offset + key_size;
        return ret;
    }

    file_metadata deserialize_file_metadata(uint8_t* raw_value, size_t size){
        file_metadata ret;
        size_t current_offset = 0;
        
        if (size && ((current_offset + sizeof(ret.file_size)) > size)) {return ret;}
        memcpy(&(ret.file_size), (raw_value + current_offset), sizeof(ret.file_size));
        current_offset = current_offset + sizeof(ret.file_size);

        if (size && ((current_offset + sizeof(ret.scope)) > size)) {return ret;}
        memcpy(&(ret.scope), (raw_value + current_offset), sizeof(ret.scope));
        current_offset = current_offset + sizeof(ret.scope);

        size_t key_size;
        if (size && ((current_offset + sizeof(key_size)) > size)) {return ret;}
        memcpy(&(key_size), (raw_value + current_offset), sizeof(key_size));
        current_offset = current_offset + sizeof(key_size);

        if (size && ((current_offset + key_size) > size)) {
            cerr << "Unable to deserialize entirely file_metadata (key string too long)" << endl;
            return ret;
        }
        std::string key(((char *)(raw_value + current_offset)), key_size);
        ret.key = key;
        
        return ret;
    }
}