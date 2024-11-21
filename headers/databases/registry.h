
#include <string>
#include <mutex>
#include <tuple>
#include "LMDB_wrapper.h"

class DatabasesRegistry
{
    private:
        static Singleton* pinstance_;
        static std::mutex mutex_;

    protected:

         DatabasesRegistry(){}
        ~DatabasesRegistry() {}


    public:
        DatabasesRegistry(DatabasesRegistry &other) = delete;
        void operator=(const DatabasesRegistry &) = delete;
        static DatabasesRegistry *GetInstance();
        LMDBWrapper& getOwnerDatabase(const std::string& value); 
}