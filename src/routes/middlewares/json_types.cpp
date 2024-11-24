
#include "routes/middlewares/json_types.h"

using namespace std;

const char* get_num_type_str(const crow::json::num_type nt)
{
    switch (nt)
    {
        case crow::json::num_type::Signed_integer: return "Signed_integer";
        case crow::json::num_type::Unsigned_integer: return "Unsigned_integer";
        case crow::json::num_type::Floating_point: return "Floating_point";
        case crow::json::num_type::Null: return "Null";
        case crow::json::num_type::Double_precision_floating_point: return "Double_precision_floating_point";
        default: return "Unknown Number Type";
    }
}

const char* json_type_str(const crow::json::rvalue& json){
    switch (json.t())
    {
        case crow::json::type::Number:
            return get_num_type_str(json.nt());

        default:
            return crow::json::get_type_str(json.t());
    }
}