#ifndef MIDDLEWARES_AUTH_MIDDLEWARES_H
#define MIDDLEWARES_AUTH_MIDDLEWARES_H

#define CROW_USE_BOOST 1

#include <boost/asio.hpp>
#include <crow.h>
#include "LMDB_wrapper.h"
#include "databases/registry.h"
#include "databases/owners.h"

int check_admin_credentials(const crow::request& req);
struct AdminAreaGuard : crow::ILocalMiddleware
{
    struct context
    {};

    void before_handle(crow::request& req, crow::response& res, context&)
    {
        int admin_check = check_admin_credentials(req);
        if (admin_check > 200)
        {
            res.code = admin_check;
            res.end();
        }
    }

    void after_handle(crow::request&, crow::response&, context&)
    {}
};


int check_owner_credentials(const crow::request& req, const std::string owner);
struct OwnerAreaGuard : crow::ILocalMiddleware
{
    struct context
    {};

    void before_handle(crow::request&, crow::response&, context&)
    {

    }

    void after_handle(crow::request&, crow::response&, context&)
    {}
};

#endif //MIDDLEWARES_AUTH_MIDDLEWARES_H