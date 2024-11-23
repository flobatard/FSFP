#ifndef FSFP_APP_TYPES_H
#define FSFP_APP_TYPES_H

#define CROW_USE_BOOST 1

#include <boost/asio.hpp>
#include <crow.h>
#include "routes/middlewares/auth_middlewares.h" 

#define FSFP_APP_TYPE crow::App<AdminAreaGuard, OwnerAreaGuard>


#endif //FSFP_APP_TYPES_H