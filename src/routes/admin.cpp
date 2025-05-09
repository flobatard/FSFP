#include "routes/admin.h"
#include "databases/files.h"
#include "databases/owners.h"
#include "databases/registry.h"
#include "LMDB_wrapper.h"
#include "routes/middlewares/json_types.h"
#include "routes/middlewares/auth_middlewares.h"
#include "fsfp_utils.h"
#include "cstdlib"

using namespace std;
namespace fs = std::filesystem;

#define CROW_ADMIN_ROUTE(app, route) \
    CROW_ROUTE(app, route).CROW_MIDDLEWARES(app, AdminAreaGuard)

int admin_routes(FSFPApp& app)
{   
    CROW_ADMIN_ROUTE(app, "/admin/owner/<string>").methods(crow::HTTPMethod::PUT)([](const crow::request& req, crow::response& res, string owner){
        crow::json::rvalue rjson_body = crow::json::load(req.body.c_str(), req.body.size());
        fsfp::db::owner_metadata owner_m;

        if (owner == "root" || owner == "admin")
        {
            res.code = 400;
            res.end("Invalid owner name, owner name root and admin are only for the admin user");
            return;
        }

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
        if (db_registry->getOwnerDatabase(owner) == NULL)
        {
            db_registry->addOwnerToRegistry(owner);
        }
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

        fsfp::db::owner_metadata owner_m;

        int rc = fsfp::db::owner_get(lmdb, owner, owner_m);
        if (rc){
            res.code = 404;
            res.end();
            return;
        }
        db_registry->removeOwner(owner);

        fs::path owner_file_path = "data";
        owner_file_path = owner_file_path / "files" / owner;
        fs::remove_all(owner_file_path);
        rc = fsfp::db::owner_del(lmdb, owner);
        if (rc){
            res.code = 500;
            res.end("Possible inconsistency introduced");
            return;
        }
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

    // Admin root (admin access to his as root owner)
    CROW_ADMIN_ROUTE(app, "/admin/root/me")([](const crow::request&, crow::response& res){
        crow::json::wvalue wjson_body({{"max_data_size", "Inf"},
                                        {"active", 1}});
        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.end(wjson_body.dump());
    });

    CROW_ADMIN_ROUTE(app, "/admin/root/search")([](const crow::request&, crow::response& res){
        DatabasesRegistry* db_registry = DatabasesRegistry::GetInstance();
        LMDBWrapper* lmdb = db_registry->getRootDatabase();
        std::vector<std::string> files = lmdb->keys();

        crow::json::wvalue wjson_body;
        wjson_body["files"] = files;

        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.end(wjson_body.dump());
    });

    CROW_ADMIN_ROUTE(app, "/admin/root/search/<path>")([](const crow::request&, crow::response& res, std::string search_path){
        DatabasesRegistry* db_registry = DatabasesRegistry::GetInstance();
        LMDBWrapper* lmdb = db_registry->getRootDatabase();
        std::vector<std::string> files = lmdb->search_keys_starts_with(search_path);

        crow::json::wvalue wjson_body;
        wjson_body["files"] = files;

        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.end(wjson_body.dump());
    });

    CROW_ADMIN_ROUTE(app, "/admin/root/metadata/<path>")([](const crow::request&, crow::response& res, std::string path){
        DatabasesRegistry* db_registry = DatabasesRegistry::GetInstance();
        LMDBWrapper* lmdb = db_registry->getRootDatabase();

        fsfp::db::file_metadata file_m;
        int rc = file_get(lmdb, path, file_m);
        if (rc)
        {
            res.code = 404;
            res.end("Unable to metadata of your file");
            return;
        }
        crow::json::wvalue wjson_body({{"file_size", file_m.file_size},
                                        {"scope", code_scope_to_str(file_m.scope)},
                                        {"key", file_m.key}});
        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.end(wjson_body.dump());
    });

    CROW_ADMIN_ROUTE(app, "/admin/root/metadata/<path>").methods(crow::HTTPMethod::PUT)([](const crow::request& req, crow::response& res, std::string path){
        DatabasesRegistry* db_registry = DatabasesRegistry::GetInstance();
        LMDBWrapper* lmdb = db_registry->getRootDatabase();
        crow::json::rvalue rjson_body = crow::json::load(req.body.c_str(), req.body.size());
        // TODO Transactionnality issue here not managed yet
        // Might lead on rare cases with wrong file_size
        // We should handle most critical memory limits with syscall and not file_metadata so for now 
        // No worries
        fsfp::db::file_metadata file_metadata;
        int rc = file_get(lmdb, path, file_metadata);
        if (rc)
        {
            res.code = 404;
            res.end("File not found");
            return;
        }

        if (rjson_body.has("key") && (rjson_body["key"].t() == crow::json::type::String))
        {
            file_metadata.key = rjson_body["key"].s();
        }

        if (rjson_body.has("scope") && (rjson_body["scope"].t() == crow::json::type::String))
        {
            string scope_str = rjson_body["scope"].s();
            unsigned int scope_code = str_scope_to_code(scope_str);
            if (scope_code == 9999)
            {
                res.code = 400;
                res.end("Invalid scope: sould be public, private or PROTECTED, got : " + scope_str);
                return;
            }
            file_metadata.scope = scope_code;
        }

        rc = file_put(lmdb, path, file_metadata);
        if (rc)
        {
            res.code = 500;
            res.end("Unable to put the data");
            return;
        }

        res.code = 200;
        res.end();
    });

    CROW_ADMIN_ROUTE(app, "/admin/root/unsafe/metadata/<path>").methods(crow::HTTPMethod::DELETE)([](const crow::request&, crow::response& res, std::string path){
        DatabasesRegistry* db_registry = DatabasesRegistry::GetInstance();
        LMDBWrapper* lmdb = db_registry->getRootDatabase();
        int rc = fsfp::db::file_del(lmdb, path);
        fs::path file_path = owner_file_path("root", path);
        fs::remove(file_path);
        if (rc)
        {
            res.code = 404;
            res.end("Unable to locate metadata file");
            return;
        }

        res.code = 200;
        res.end();
    });

    CROW_ADMIN_ROUTE(app, "/admin/root/global_metadata").methods(crow::HTTPMethod::PUT)([](const crow::request& , crow::response& ){

    });

    CROW_ADMIN_ROUTE(app, "/admin/root/files/<path>")([](const crow::request&, crow::response& res, std::string path){
        fs::path ret = owner_file_path("root", path);
        res.set_static_file_info(ret);
        res.end();
    });

    CROW_ADMIN_ROUTE(app, "/admin/root/files/<path>").methods(crow::HTTPMethod::PUT)([](const crow::request& req, crow::response& res, std::string path){
        DatabasesRegistry* db_registry = DatabasesRegistry::GetInstance();
        LMDBWrapper* lmdb = db_registry->getRootDatabase();
        crow::multipart::message_view file_message(req);
        if (file_message.parts.size() == 0)
        {
            res.code = 400;
            res.end("No file provided");
        }
        std::string scope;
        std::string key = "";
        crow::multipart::part_view part;
        
        int rc = parse_message_view(file_message, part, scope, key);
        if (rc){
            if (rc == 1) {res.code = 400; res.end("No file provided (the key must be file for where the file is located in the multipart form)");}
            else if (rc == 2) {res.code = 400; res.end("No scope provided");}
            else {res.code = 500; res.end("Unknown error");}
            return;
        }
        fsfp::db::file_metadata file_m;
        file_m.scope = str_scope_to_code(scope);
        if (file_m.scope == 9999)
        {
            res.code = 400;
            res.end("Invalid scope: sould be public, private or protected, got : " + scope);
        }

        fs::path file_path = "root";
        file_path = file_path / path;
        rc = upload_file(file_path, part);
        if (rc)
        {
            res.code = rc;
            std::string ret;
            if (rc == 400) {ret = "Bad file path (probably '..' in path, you should not do that)";}
            if (rc == 500) {ret = "Failed to upload file";}
            res.code = rc;
            res.end(ret);
        }

        file_m.file_size = part.body.size();
        file_m.key = key;
        
        std::string admin_path = "admin/root/files/" + path;
        std::string guest_path = "guest/";
        switch(file_m.scope)
        {
            case fsfp::types::scope_type::PUBLIC:
                guest_path = guest_path + "root/" + path;
                break;
            case fsfp::types::scope_type::PROTECTED:
                guest_path = guest_path + "root/" + path + "?key=" + key;
                break;
            case fsfp::types::scope_type::PRIVATE:
                guest_path = "";
                break;
            default:
                guest_path = "";
        }

        fsfp::db::file_put(lmdb, path, file_m);

        crow::json::wvalue wjson_body({
                                        {"scope", code_scope_to_str(file_m.scope)},
                                        {"key", file_m.key},
                                        {"path", path},
                                        {"guest_path", guest_path},
                                        {"admin_path", admin_path}});
        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.end(wjson_body.dump());
    });

    CROW_ADMIN_ROUTE(app, "/admin/root/files/<path>").methods(crow::HTTPMethod::DELETE)([](const crow::request& , crow::response& res, std::string path){
        DatabasesRegistry* db_registry = DatabasesRegistry::GetInstance();
        LMDBWrapper* lmdb = db_registry->getRootDatabase();
        
        fsfp::db::file_metadata file_m;
        int rc = fsfp::db::file_get(lmdb, path, file_m);
        if (rc)
        {
            res.code = rc;
            res.end("File not found");
            return;
        }
        fs::path ret = owner_file_path("root", path);
        if (!fs::remove(ret))
        {
            res.code = 500;
            res.end("Unable to remove the file");
            return;
        }

        rc = fsfp::db::file_del(lmdb, path);
        if (rc)
        {
            res.code = 500;
            CROW_LOG_WARNING << "Unable to delete metadata of file: " << path;
            res.end("Unable to delete metadata for the file, however file was deleted successfully");
            return;
        }

        res.code = 200;
        res.end();
    });

    // Admin access to all owners data
    return 0;
}