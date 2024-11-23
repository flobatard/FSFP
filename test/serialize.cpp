#include "databases/file.h"
#include "databases/owners.h"
#include <iostream>

using namespace fsfp::db;
using namespace std;

int main()
{
    owner_metadata owner_m;
    owner_m.active = 2;
    owner_m.max_data_size = 10;
    uint8_t* serialized = serialize_owner_metadata(owner_m);
    std::string ser_as_str((char*) serialized, size_of_owner_metadata(owner_m));
    owner_metadata after = deserialize_owner_metadata(serialized);
    cout << "Before: " << owner_m.active << " " << owner_m.max_data_size << endl;
    cout << "After: " << after.active << " " << after.max_data_size << endl;

    if (owner_m.active != after.active || owner_m.max_data_size != after.max_data_size )
    {
        throw ("Test failed");
    }
    
    file_metadata file_m;
    file_m.file_size = 40;
    file_m.scope = 50;
    file_m.key = "JeSuisUneClé";

    uint8_t* file_serialized = serialize_file_metadata(file_m);
    std::string f_ser_as_str((char*) file_serialized, size_of_file_metadata(file_m));
    file_metadata after_f = deserialize_file_metadata(file_serialized);
    cout << "Before: " << file_m.file_size << " " << file_m.scope << " " << file_m.key << endl;
    cout << "After: " << after_f.file_size << " " << after_f.scope << " " << after_f.key << endl;

    if (after_f.file_size != file_m.file_size || after_f.scope != file_m.scope || after_f.key != file_m.key )
    {
        throw ("Test failed");
    }
    
    free(serialized);
    free(file_serialized);
    return 0;
}