#include "routes/middlewares/auth_middlewares.h" 

using namespace std;

int check_admin_credentials(const crow::request& req){
    string admin_key("DUMMY_ADMIN_KEY"); //getenv("FSFP_ADMIN_KEY");
    string user_key = crow::get_header_value(req.headers, "fsfp_admin_key");

    if (user_key == ""){return 401;}
    if (admin_key != user_key){return 403;}
    return 200;
}

int check_owner_credentials(const crow::request& req){
    string owner_key("DUMMY_OWNER_KEY"); //getenv("FSFP_ADMIN_KEY");
    string user_key = crow::get_header_value(req.headers, "fsfp_owner_key");

    if (user_key == ""){return 401;}
    if (owner_key != user_key){return 403;}
    return 0;
}