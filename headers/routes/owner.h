#ifndef ROUTES_OWNER_H
#define ROUTES_OWNER_H

#define CROW_USE_BOOST 1

#include <utility>
#include <boost/asio.hpp>
#include <crow.h>
#include "fsfp_app_types.h"

int owner_routes(FSFPApp& app);

#endif //ROUTES_OWNER_H