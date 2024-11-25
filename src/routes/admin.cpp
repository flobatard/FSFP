#include "routes/admin.h"
#include "databases/owners.h"
#include "databases/registry.h"
#include "LMDB_wrapper.h"
#include "routes/middlewares/json_types.h"
#include "routes/middlewares/auth_middlewares.h"
#include "cstdlib"

using namespace std;

#define CROW_ADMIN_ROUTE(app, route) \
    CROW_ROUTE(app, route).CROW_MIDDLEWARES(app, AdminAreaGuard)

int admin_routes(FSFP_APP_TYPE& app)
{   
    CROW_ADMIN_ROUTE(app, "/admin/owner/<string>").methods(crow::HTTPMethod::PUT)([](const crow::request& req, crow::response& res, string owner){
        crow::json::rvalue rjson_body = crow::json::load(req.body.c_str(), req.body.size());
        fsfp::db::owner_metadata owner_m;
        if (!rjson_body.has("max_data_size") || !(rjson_body["max_data_size"].nt() == crow::json::num_type::Unsigned_integer))
        {
            res.code = 400;
            res.end("Invalid max_size_data attribute");
            return;
        }

        if (!rjson_body.has("active") || !(rjson_body["active"].nt() == crow::json::num_type::Unsigned_integer))
        {
            res.code = 400;
            res.end("Invalid active attribute");
            return;
        }

        owner_m.max_data_size = rjson_body["max_data_size"].u();
        owner_m.active = rjson_body["active"].u();

        DatabasesRegistry* db_registry = DatabasesRegistry::GetInstance();
        LMDBWrapper* lmdb = db_registry->getRegistryDatabase();

        fsfp::db::owner_put(lmdb, owner, owner_m);
        db_registry->addOwnerToRegistry(owner);
        crow::json::wvalue wjson_body(rjson_body);
        
        res.code = 201;
        res.set_header("Content-Type", "application/json");
        res.end(wjson_body.dump());
    });

    CROW_ADMIN_ROUTE(app, "/admin/owner/<string>").methods(crow::HTTPMethod::GET)([](const crow::request&, crow::response& res, string owner){
        DatabasesRegistry* db_registry = DatabasesRegistry::GetInstance();
        LMDBWrapper* lmdb = db_registry->getRegistryDatabase();

        fsfp::db::owner_metadata owner_m;
        int rc = fsfp::db::owner_get(lmdb, owner, owner_m);

        if (rc)
        {
            res.code = rc;
            res.end("");
            return;
        }
        
        crow::json::wvalue wjson_body({{"max_data_size", owner_m.max_data_size},
                                        {"active", owner_m.active}});
        
        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.end(wjson_body.dump());
    });

    CROW_ADMIN_ROUTE(app, "/admin/owner/<string>").methods(crow::HTTPMethod::DELETE)([](const crow::request&, crow::response& res, string owner){
        DatabasesRegistry* db_registry = DatabasesRegistry::GetInstance();
        LMDBWrapper* lmdb = db_registry->getRegistryDatabase();

        int rc = fsfp::db::owner_del(lmdb, owner);
        if (rc){
            res.code = 404;
            res.end();
            return;
        }
        db_registry->removeOwner(owner);
        
        res.code = 200;
        res.end();
    });


    CROW_ADMIN_ROUTE(app, "/admin/owners/all").methods(crow::HTTPMethod::GET)([](const crow::request&, crow::response& res){
        DatabasesRegistry* db_registry = DatabasesRegistry::GetInstance();
        LMDBWrapper* lmdb = db_registry->getRegistryDatabase();

        std::vector<std::string> owners = lmdb->keys();

        crow::json::wvalue wjson_body;
        wjson_body["owners"] = owners;
        
        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.end(wjson_body.dump());
    });





    return 0;
}