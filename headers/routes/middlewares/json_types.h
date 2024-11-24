#ifndef MIDDLEWARES_JSON_TYPES_H
#define MIDDLEWARES_JSON_TYPES_H

#define CROW_USE_BOOST 1

#include <boost/asio.hpp>
#include <crow.h>

const char* json_type_str(const crow::json::rvalue& json);
const char* get_num_type_str(const crow::json::num_type nt);

#endif //MIDDLEWARES_JSON_TYPES_H