
#include "routes/guest.h"
#include "fsfp_utils.h"
#include "databases/registry.h"
#include "databases/files.h"
#include <filesystem>

#define RETRIEVE_LMDB(owner, res) \
    DatabasesRegistry* registry = DatabasesRegistry::GetInstance();\
    LMDBWrapper* lmdb = registry->getOwnerDatabase(owner);\
    if (lmdb == NULL)\
    {\
        res.code = 404;\
        res.end("Unknown owner");\
        return;\
    }

using namespace std;
namespace fs = std::filesystem;

int check_file(LMDBWrapper* lmdb, const std::string& path, const std::string& key, std::string& error_message)
{
    fsfp::db::file_metadata file_m;
    int rc = file_get(lmdb, path, file_m);
    if (rc)
    {
        error_message = "File not found";
        return 404;
    }
    
    switch(file_m.scope)
    {
        case fsfp::types::scope_type::PUBLIC:
            break;

        case fsfp::types::scope_type::PROTECTED:
            // Might change for standard http error code for bad Credentials
            // The idea here is more to prevent attacker from guessing filenames for some owners
            if (key.size() == 0)
            {
                error_message = "File not found";
                rc = 404;
            }
            else if (key != file_m.key)
            {
                error_message = "File not found";
                rc = 404;
            }
            break;

        case fsfp::types::scope_type::PRIVATE:
            error_message = "File not found";
            rc = 404;
            break;
        
        default:
            error_message = "File not found";
            rc = 404;
    }
    return rc;
}

int guest_routes(FSFPApp& app){

    CROW_ROUTE(app, "/guest/root/<path>")([](const crow::request& req, crow::response& res, std::string path){
        DatabasesRegistry* registry = DatabasesRegistry::GetInstance();
        LMDBWrapper* lmdb = registry->getRootDatabase();
        char* c_key = req.url_params.get("key");
        std::string key( c_key ? c_key : "");
        std::string error_message = "";

        int rc = check_file(lmdb, path, key, error_message);
        if (rc)
        {
            res.code = rc;
            res.end(error_message);
            return;
        }

        fs::path file_path = owner_file_path("root", path);
        res.set_static_file_info(file_path);
        res.end();
    });

    CROW_ROUTE(app, "/guest/<string>/<path>")([](const crow::request& req, crow::response& res, std::string owner, std::string path){
        RETRIEVE_LMDB(owner, res)

        char* c_key = req.url_params.get("key");
        std::string key( c_key ? c_key : "");
        std::string error_message = "";

        int rc = check_file(lmdb, path, key, error_message);
        if (rc)
        {
            res.code = rc;
            res.end(error_message);
            return;
        }

        fs::path file_path = owner_file_path(owner, path);
        res.set_static_file_info(file_path);
        res.end();
    });

    return 0;
}