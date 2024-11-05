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

    //set the port, set the app to run on multiple threads, and run the app
    app.port(8000).multithreaded().run();
    return 0;
}
