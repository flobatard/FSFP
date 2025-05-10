#include "config.h"
#include <iostream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

FSFPConfig::FSFPConfig() {
    secretKey = "";
}

FSFPConfig::~FSFPConfig() {
}

int FSFPConfig::loadConfig() {
    cout << "Loading config..." << endl;

    secretKey = "dummysecretkey";

    cout << "Config loaded" << endl;
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