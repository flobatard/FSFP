#include "fsfp_utils.h"
#include <filesystem>

#ifdef _WIN32
    #define DATA_FILES "data\\files"
#else
    #define DATA_FILES "data/files"
#endif

#define SCOPE_PUBLIC 0
#define SCOPE_KEY_PROTECTED 1
#define SCOPE_PRIVATE 2

using namespace std;
namespace fs = std::filesystem;

unsigned int str_scope_to_code(const string& scope){
    if (scope == "public") {return SCOPE_PUBLIC;}
    if (scope == "protected") {return SCOPE_KEY_PROTECTED;}
    if (scope == "private") {return SCOPE_PRIVATE;}
    return 9999;
}

string code_scope_to_str(const unsigned int scope){
    switch(scope)
    {
        case SCOPE_PUBLIC:
            return "public";
        case SCOPE_KEY_PROTECTED:
            return "protected";
        case SCOPE_PRIVATE:
            return "private";
        default:
            return "unknown";
    }
}

int print_headers(crow::multipart::mph_view_map headers)
{
    for (auto const& i : headers) {
        std::cout << "'" << i.first << "': '" << i.second.value << "'" << std::endl;
    }
    return 0;
}

int print_values(crow::multipart::mp_view_map values)
{
    for (auto const& i : values) {
        std::cout << "'" << i.first << "': '" << i.second.body << "'" << std::endl;
    }
    return 0;
}

int parse_message_view(const crow::multipart::message_view& message_view, crow::multipart::part_view& part, std::string scope, std::string& key)
{
    int fileFlag = 1;
    int scopeFlag = 2;
    for (auto const& i : message_view.part_map)
    {
        if (i.first == "file"){
            part = i.second;
            fileFlag = 0;
        }
        else if (i.first == "scope")
        {
            scope = i.second.body;
            scopeFlag = 0;
        }
        else if (i.first == "key")
        {
            key = i.second.body;
        }
        else
        {
            CROW_LOG_WARNING << "Unknown multipart key: " << i.first;
        }
    }
    return fileFlag && scopeFlag;
}

fs::path owner_file_path(const string& owner, const string& path)
{
    fs::path ret = DATA_FILES;
    ret = ret / owner / path;
    return ret;
}

int upload_file(const string& path, const crow::multipart::part_view& part){
    if (path.find("..") != string::npos)
    {
        return 400;
    }

    fs::path file_path(DATA_FILES);
    file_path = file_path / path;
    const fs::path parent_path = file_path.parent_path();
    fs::create_directories(parent_path);

    // Create a new file with the extracted file name and write file contents to it
    std::ofstream out_file(file_path);
    if (!out_file)
    {
        cout << " Write to file failed" << endl;
        return 500;
    }
    out_file << part.body;
    out_file.close();
    return 0;
}