#ifndef FSFP_CONFIG_H
#define FSFP_CONFIG_H

#include <mutex>
#include <string>

class FSFPConfig
{
    public:
        static FSFPConfig* instance;
        static std::mutex mtx;
        FSFPConfig(FSFPConfig &other) = delete;
        void operator=(const FSFPConfig &) = delete;
        static FSFPConfig* GetInstance();
        static void DestroyInstance();
        int readConfig();
        int loadConfig();
        std::string& getSecretKey();
        FSFPConfig();
        ~FSFPConfig();
        

    protected:
        std::string secretKey;
};

#endif //FSFP_CONFIG_H