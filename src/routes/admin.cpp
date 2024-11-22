#include "routes/admin.h"
#include "routes/middlewares/auth_middlewares.h"
#include "cstdlib"

using namespace std;

#define CROW_ADMIN_ROUTE(app, route) \
    CROW_ROUTE(app, route).CROW_MIDDLEWARES(app, AdminAreaGuard)

int admin_routes(FSFP_APP_TYPE& app)
{   
    CROW_ADMIN_ROUTE(app, "/admin/owner")([](){
        return crow::response(200);
    });


    return 0;
}