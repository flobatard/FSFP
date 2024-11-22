#include "routes/owner.h"

using namespace std;

#define CROW_OWNER_ROUTE(app, route) \
    CROW_ROUTE(app, route).CROW_MIDDLEWARES(app, OwnerAreaGuard)

int owner_routes(FSFP_APP_TYPE& app){
    CROW_ROUTE(app, "/owner/me")
    .CROW_MIDDLEWARES(app, OwnerAreaGuard)([](){
        return crow::response(200);
    });
    return 0;
}