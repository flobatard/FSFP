#define CROW_USE_BOOST 1

#include <boost/asio.hpp>
#include <crow.h>

#include "filesystem"
#include <iostream>
#include <string>

int upload_file(const std::string& path, const crow::multipart::part_view& part);