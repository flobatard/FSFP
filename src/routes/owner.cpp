#include "routes/owner.h"
#include <filesystem>
#include "fsfp_utils.h"
#include "LMDB_wrapper.h"
#include "databases/registry.h"
#include "databases/owners.h"
#include "databases/files.h"

using namespace std;
namespace fs = std::filesystem;

#define CROW_OWNER_ROUTE(app, route) \
    CROW_ROUTE(app, route)

#define RETRIEVE_LMDB(owner, res) \
    DatabasesRegistry* registry = DatabasesRegistry::GetInstance();\
    LMDBWrapper* lmdb = registry->getOwnerDatabase(owner);\
    if (lmdb == NULL)\
    {\
        res.code = 404;\
        res.end("Unknown owner");\
        return;\
    }

#define CHECK_OWNER_CREDENTIALS(req, owner, res) \
    if (int _res_code = check_owner_credentials(req, owner))\
    {\
        res.code = _res_code;\
        res.end();\
        return;\
    }


int owner_routes(FSFPApp& app){
    CROW_OWNER_ROUTE(app, "/owner/<string>/me")([](const crow::request& req, crow::response& res, std::string owner){
        CHECK_OWNER_CREDENTIALS(req, owner, res)
        DatabasesRegistry* registry = DatabasesRegistry::GetInstance();
        LMDBWrapper* lmdb = registry->getRegistryDatabase();
        fsfp::db::owner_metadata owner_m;
        if (int rc = fsfp::db::owner_get(lmdb, owner, owner_m))
        {
            res.code = rc;
            res.end("Unknown user");
            return;
        }

        crow::json::wvalue wjson_body({{"max_data_size", owner_m.max_data_size},
                                        {"active", owner_m.active}});
        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.end(wjson_body.dump());
    });

    CROW_OWNER_ROUTE(app, "/owner/<string>/search")([](const crow::request&req, crow::response& res, std::string owner){
        CHECK_OWNER_CREDENTIALS(req, owner, res)
        RETRIEVE_LMDB(owner, res)
        std::vector<std::string> files = lmdb->keys();

        crow::json::wvalue wjson_body;
        wjson_body["files"] = files;

        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.end(wjson_body.dump());
    });

    CROW_OWNER_ROUTE(app, "/owner/<string>/search/<path>")([](const crow::request&req, crow::response& res, std::string owner, std::string search_path){
        CHECK_OWNER_CREDENTIALS(req, owner, res)
        RETRIEVE_LMDB(owner, res)
        std::vector<std::string> files = lmdb->search_keys_starts_with(search_path);

        crow::json::wvalue wjson_body;
        wjson_body["files"] = files;

        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.end(wjson_body.dump());
    });

    CROW_OWNER_ROUTE(app, "/owner/<string>/metadata/<path>")([](const crow::request& req, crow::response& res, std::string owner, std::string path){
        CHECK_OWNER_CREDENTIALS(req, owner, res)
        RETRIEVE_LMDB(owner, res)

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

    CROW_OWNER_ROUTE(app, "/owner/<string>/metadata/<path>").methods(crow::HTTPMethod::PUT)([](const crow::request& req, crow::response& res, std::string owner, std::string path){
        CHECK_OWNER_CREDENTIALS(req, owner, res)
        RETRIEVE_LMDB(owner, res)
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

    CROW_OWNER_ROUTE(app, "/owner/<string>/unsafe/metadata/<path>").methods(crow::HTTPMethod::DELETE)([](const crow::request& req, crow::response& res, std::string owner, std::string path){
        CHECK_OWNER_CREDENTIALS(req, owner, res)
        RETRIEVE_LMDB(owner, res)
        int rc = fsfp::db::file_del(lmdb, path);
        fs::path file_path = owner_file_path(owner, path);
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

    CROW_OWNER_ROUTE(app, "/owner/<string>/global_metadata").methods(crow::HTTPMethod::PUT)([](const crow::request& , crow::response& , std::string ){

    });

    CROW_OWNER_ROUTE(app, "/owner/<string>/files/<path>")([](const crow::request& req, crow::response& res, std::string owner, std::string path){
        CHECK_OWNER_CREDENTIALS(req, owner, res)
        fs::path ret = owner_file_path(owner, path);
        cout << ret << endl;
        res.set_static_file_info(ret);
        res.end();
    });

    CROW_OWNER_ROUTE(app, "/owner/<string>/files/<path>").methods(crow::HTTPMethod::PUT)([](const crow::request& req, crow::response& res, std::string owner, std::string path){
        CHECK_OWNER_CREDENTIALS(req, owner, res)
        RETRIEVE_LMDB(owner, res)
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

        fs::path file_path = owner;
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
        
        std::string owner_path = "owner/" + owner + "/files/" + path;
        std::string guest_path = "guest/";
        switch(file_m.scope)
        {
            case fsfp::types::scope_type::PUBLIC:
                guest_path = guest_path + owner + "/" + path;
                break;
            case fsfp::types::scope_type::PROTECTED:
                guest_path = guest_path + owner + "/" + path + "?key=" + key;
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
                                        {"owner_path", owner_path}});
        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.end(wjson_body.dump());
    });

    CROW_OWNER_ROUTE(app, "/owner/<string>/files/<path>").methods(crow::HTTPMethod::DELETE)([](const crow::request& req , crow::response& res, std::string owner, std::string path){
        CHECK_OWNER_CREDENTIALS(req, owner, res)
        RETRIEVE_LMDB(owner, res)

        fsfp::db::file_metadata file_m;
        int rc = fsfp::db::file_get(lmdb, path, file_m);
        if (rc)
        {
            res.code = rc;
            res.end("File not found");
            return;
        }
        fs::path ret = owner_file_path(owner, path);
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
    return 0;
}