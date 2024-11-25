#include "http_server.h"
#include "fsfp_utils.h"
#include "routes/admin.h"
#include "routes/owner.h"
#include "fsfp_app_types.h"

using namespace std;
namespace fs = std::filesystem;

int run_server()
{
    FSFP_APP_TYPE app; //define your crow application

    admin_routes(app);
    owner_routes(app);

    //define your endpoint at the root directory
    CROW_ROUTE(app, "/")([](){
        string ret = "Boost version: " + to_string(BOOST_VERSION / 100000)
          + "."
          + to_string(BOOST_VERSION / 100 % 1000)
          + "."
          + to_string(BOOST_VERSION % 100) ;
        return ret;
    });

    CROW_ROUTE(app, "/test/<int>")([](const crow::request& req, int a){

        std::string user_agent = crow::get_header_value(req.headers, "User-Agent");
        
        char* ch_key = req.url_params.get("key");
        char* ch_id = req.url_params.get("id");
        string key;
        string id;
        if (req.url_params.get("key"))
        {
            key = string(ch_key);
        }
        if (req.url_params.get("id"))
        {
            id = string(ch_id);
        }
        return "Id= " + id + " Key= " + key + " A=" + to_string(a) + " UA:" + user_agent;
    });

    CROW_ROUTE(app, "/test/file/<path>")([](const crow::request&, crow::response& res, string path){
        fs::path file_path("data");
        file_path = file_path / "files" / path;
        res.set_static_file_info(file_path);
        res.end();
    });

    CROW_ROUTE(app, "/test/upload/<path>").methods(crow::HTTPMethod::Post)([](const crow::request& req, string path){
        crow::multipart::message_view file_message(req);
        if (file_message.parts.size() == 0)
        {
            return crow::response(400);
        }

        crow::multipart::part_view part = file_message.parts[0];
        const int err = upload_file(path, part);
        if (err)
        {
            return crow::response(err);
        }
        return crow::response(200);
    });

    //set the port, set the app to run on multiple threads, and run the app
    app.port(5000).multithreaded().run();
    return 0;
}
