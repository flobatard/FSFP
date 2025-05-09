#ifndef ROUTES_GUEST_H
#define ROUTES_GUEST_H

#define CROW_USE_BOOST 1

#include <utility>
#include <boost/asio.hpp>
#include <crow.h>

#include <string>
#include "LMDB_wrapper.h"
#include "fsfp_app_types.h"

int retrieve_file(LMDBWrapper* lmbd, const std::string& owner, const std::string& path, const std::string& key, std::string& error_message);
int guest_routes(FSFPApp& app);

#endif //ROUTES_GUEST_H