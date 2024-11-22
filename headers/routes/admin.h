#ifndef ROUTES_ADMIN_H
#define ROUTES_ADMIN_H

#define CROW_USE_BOOST 1

#include <boost/asio.hpp>
#include <crow.h>


int check_admin_credentials(const crow::request& req);
int admin_routes(crow::SimpleApp& app);

#endif //ROUTES_ADMIN_H
