#include "routes/admin.h"
#include "databases/owners.h"
#include "databases/registry.h"
#include "routes/middlewares/json_types.h"
#include "routes/middlewares/auth_middlewares.h"
#include "cstdlib"

using namespace std;

#define CROW_ADMIN_ROUTE(app, route) \
    CROW_ROUTE(app, route).CROW_MIDDLEWARES(app, AdminAreaGuard)

int admin_routes(FSFP_APP_TYPE& app)
{   
    CROW_ADMIN_ROUTE(app, "/admin/owner").methods(crow::HTTPMethod::Post)([](const crow::request& req){
        crow::json::rvalue rjson_body = crow::json::load(req.body.c_str(), req.body.size());

        if (rjson_body.has("toto")){
            cout << "toto: " << rjson_body["toto"].s() << endl;
        }
        if (rjson_body.has("max_data_size")
            && rjson_body["max_data_size"].t() == crow::json::type::String)
        {
            cout << "max_data_size" << rjson_body["max_data_size"].s() << endl;
        }

        if (rjson_body.has("max_data_size") 
            && rjson_body["max_data_size"].t() == crow::json::type::Number 
            && rjson_body["max_data_size"].nt() == crow::json::num_type::Unsigned_integer)
        {
            cout << "max_data_size" << rjson_body["max_data_size"].u() << endl;
        }
        
        
        crow::json::wvalue x ({{"zmessage", "Hello, World!"}});
        x["message2"] = "Hello World.. Again!";
        return x;
    });


    return 0;
}