#ifndef ROUTES_ADMIN_H
#define ROUTES_ADMIN_H

#define CROW_USE_BOOST 1

#include <boost/asio.hpp>
#include <crow.h>
#include "fsfp_app_types.h"

int admin_routes(FSFPApp& app);

#endif //ROUTES_ADMIN_H
