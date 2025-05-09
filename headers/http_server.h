#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#define CROW_MAIN
#define CROW_USE_BOOST 1

#include <utility>
#include <boost/asio.hpp>
#include <crow.h>

int run_server();

#endif //HTTP_SERVER_H