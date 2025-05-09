#ifndef FSFP_APP_TYPES_H
#define FSFP_APP_TYPES_H

#define CROW_USE_BOOST 1

#include <utility>
#include <boost/asio.hpp>
#include <crow.h>
#include "routes/middlewares/auth_middlewares.h" 
#include "routes/middlewares/log_middlewares.h"

#define FSFP_APP_TYPE crow::App<AdminAreaGuard, OwnerAreaGuard, TimeLogMiddleware>

typedef crow::App<AdminAreaGuard, OwnerAreaGuard, TimeLogMiddleware> FSFPApp;

#endif //FSFP_APP_TYPES_H