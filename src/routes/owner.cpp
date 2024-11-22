#include "routes/owner.h"

using namespace std;

#define CROW_OWNER_ROUTE(app, route) \
    CROW_ROUTE(app, route).CROW_MIDDLEWARES(app, OwnerAreaGuard)

int owner_routes(FSFP_APP_TYPE& app){
    CROW_OWNER_ROUTE(app, "/owner/me")([](){
        return crow::response(200);
    });
    return 0;
}