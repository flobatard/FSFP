#include "config.h"
#include <iostream>
#include <filesystem>
#include <crow/json.h>

using namespace std;
namespace fs = std::filesystem;

FSFPConfig::FSFPConfig() {
    secretKey = "";
}

FSFPConfig::~FSFPConfig() {
}

int FSFPConfig::loadConfig() {
    cout << "Loading config..." << endl;
    this->readConfig();

    secretKey = "dummysecretkey";

    cout << "Config loaded" << endl;
    return 0;
}

int FSFPConfig::readConfig() {
    cout << "Reading config..." << endl;
    const char* c_strConfigFileName = getenv("FSFP_CONFIG_FILE");
    string configFileName;
    if (!c_strConfigFileName)
    {
        cout << "ENV FSFP_CONFIG_FILE is NULL taking default config file config.json" << endl;
        configFileName = "config.json";
    }
    else
    {
        cout << "ENV FSFP_CONFIG_FILE: " << c_strConfigFileName << endl;
        configFileName = c_strConfigFileName;
    }



    cout << "Reading config at: " << configFileName << endl;


    return 0;
}

string& FSFPConfig::getSecretKey() {
    return secretKey;
}

FSFPConfig* FSFPConfig::GetInstance() {
    // Acquire lock before checking instance
    lock_guard<mutex> lock(FSFPConfig::mtx); // automatically releases lock
    if (FSFPConfig::instance == nullptr) {
        FSFPConfig::instance = new FSFPConfig(); // Create the instance only once
    }
    return FSFPConfig::instance;
}

void FSFPConfig::DestroyInstance() {
    delete FSFPConfig::instance;
    instance = NULL;
}


mutex FSFPConfig::mtx;
FSFPConfig* FSFPConfig::instance = nullptr;