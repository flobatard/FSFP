#include "http_server.h"

using namespace std;

int run_server()
{
    crow::SimpleApp app; //define your crow application

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
        return "Id= " + id + " Key= " + key + " A=" + to_string(a);
    });

    CROW_ROUTE(app, "/test/serve_file_explicit/<path>")([](const crow::request& req, crow::response& res, string path){
        cout << path << endl;
        res.set_static_file_info(path);
        res.end();
    });

    CROW_ROUTE(app, "/test/upload/<path>").methods(crow::HTTPMethod::Post)([](const crow::request& req, string path){
        crow::multipart::message_view file_message(req);
        cout << "DUMP" << file_message.dump() << endl;

        for (const auto& part : file_message.parts)
        {
            const auto& part_value = part;
            if (true)
            {
                cout << "Goodfile " << "l" << endl;
                // Create a new file with the extracted file name and write file contents to it
                std::ofstream out_file(path);
                if (!out_file)
                {
                    cout << " Write to file failed\n";
                    return crow::response(500); 
                }
                out_file << part_value.body;
                out_file.close();
                cout << " Contents written to " << path << '\n';
            }
            else
            {
                cout << " Value: " << part_value.body << '\n';
            }
        }
        return crow::response(200);

        if (file_message.parts.size() == 0)
        {
            return crow::response(400);
        }

        std::ofstream out_file(path);
        if (!out_file)
        {
            CROW_LOG_ERROR << " Write to file failed\n";
            return crow::response(500, "Failed to write file");
        }
        out_file << file_message.parts[0].body;

        
        out_file.close();
        CROW_LOG_INFO << " Contents written to " << path << '\n';
        return crow::response("200");
    });

    //set the port, set the app to run on multiple threads, and run the app
    app.port(8000).multithreaded().run();
    return 0;
}
