#ifndef FSFP_TYPES_H
#define FSFP_TYPES_H
#include <cstddef>

namespace fsfp::types{
    struct raw_val{
        void* data;
        size_t size;
    };
}

#endif //FSFP_TYPES_H