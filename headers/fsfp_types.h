#ifndef FSFP_UTILS_H
#define FSFP_UTILS_H
#include <cstddef>

namespace fsfp::types{
    struct raw_val{
        void* data;
        size_t size;
    };
}

#endif //FSFP_UTILS_H