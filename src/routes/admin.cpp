#include "routes/admin.h"
#include "cstdlib"

using namespace std;

struct AdminAreaGuard
{
    struct context
    {};

    void before_handle(crow::request& req, crow::response& res, context& ctx)
    {
        if (check_admin_credentials(req))
        {
            res.code = 403;
            res.end();
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx)
    {}
};

int check_admin_credentials(const crow::request& req){
    string admin_key("DUMMY_ADMIN_KEY"); //getenv("FSFP_ADMIN_KEY");

    return 0;
}

int admin_routes(crow::SimpleApp& app)
{

    CROW_ROUTE(app, "/admin/owner").methods(crow::HTTPMethod::Post)([](){
        return crow::response(400);
    });

    return 0;
}