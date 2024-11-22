#ifndef ROUTES_OWNER_H
#define ROUTES_OWNER_H

#define CROW_USE_BOOST 1

#include <boost/asio.hpp>
#include <crow.h>
#include "fsfp_app_types.h"

int owner_routes(FSFP_APP_TYPE& app);

#endif //ROUTES_OWNER_H