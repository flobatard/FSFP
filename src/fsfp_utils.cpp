#include "fsfp_utils.h"

#ifdef _WIN32
    #define SEP "\\";
#else
    #define SEP "/";
#endif

using namespace std;
namespace fs = std::filesystem;

int upload_file(const string& path, const crow::multipart::part_view& part){
    if (path.find("..") != string::npos)
    {
        return 400;
    }

    fs::path file_path("data");
    file_path = file_path / "files" / path;
    const fs::path parent_path = file_path.parent_path();
    fs::create_directories(parent_path);

    // Create a new file with the extracted file name and write file contents to it
    std::ofstream out_file(file_path);
    if (!out_file)
    {
        cout << " Write to file failed\n";
        return 500;
    }
    out_file << part.body;
    out_file.close();
    return 0;
}