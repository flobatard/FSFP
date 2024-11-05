#include "http_server.h"

int run_server()
{
    crow::SimpleApp app; //define your crow application

    //define your endpoint at the root directory
    CROW_ROUTE(app, "/")([](){
        return "Hello world TOTO";
    });

    //set the port, set the app to run on multiple threads, and run the app
    app.port(8000).multithreaded().run();
    return 0;
}
