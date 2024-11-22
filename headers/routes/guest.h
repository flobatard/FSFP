#ifndef ROUTES_GUEST_H
#define ROUTES_GUEST_H

#define CROW_USE_BOOST 1

#include <boost/asio.hpp>
#include <crow.h>


int guest_routes(crow::SimpleApp& app);

#endif //ROUTES_GUEST_H