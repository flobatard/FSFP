#ifndef FSFP_UTILS_H
#define FSFP_UTILS_H

#define CROW_USE_BOOST 1

#include <boost/asio.hpp>
#include <crow.h>

#include <iostream>
#include <string>
#include <filesystem>

unsigned int str_scope_to_code(const std::string& scope_str);
std::string code_scope_to_str(const unsigned int scope_scope);
int print_headers(crow::multipart::mph_view_map headers);
std::filesystem::path owner_file_path(const std::string& owner, const std::string& path);
int print_values(crow::multipart::mp_view_map value);
int parse_message_view(const crow::multipart::message_view& message_view, crow::multipart::part_view& part, std::string& scope, std::string& key);
int upload_file(const std::string& path, const crow::multipart::part_view& part);

#endif //FSFP_UTILS_H
