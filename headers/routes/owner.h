#ifndef ROUTES_OWNER_H
#define ROUTES_OWNER_H

#define CROW_USE_BOOST 1

#include <boost/asio.hpp>
#include <crow.h>


int check_owner_credentials(const crow::request& req);
int owner_routes();

#endif //ROUTES_OWNER_H