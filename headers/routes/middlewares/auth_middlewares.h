#ifndef MIDDLEWARES_AUTH_MIDDLEWARES_H
#define MIDDLEWARES_AUTH_MIDDLEWARES_H

#define CROW_USE_BOOST 1

#include <boost/asio.hpp>
#include <crow.h>
#include "fsfp_app_types.h"
int check_admin_credentials(const crow::request& req);
struct AdminAreaGuard : crow::ILocalMiddleware
{
    struct context
    {};

    void before_handle(crow::request& req, crow::response& res, context& ctx)
    {
        int admin_check = check_admin_credentials(req);
        if (admin_check > 200)
        {
            res.code = admin_check;
            res.end();
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx)
    {}
};


int check_owner_credentials(const crow::request& req);
struct OwnerAreaGuard : crow::ILocalMiddleware
{
    struct context
    {};

    void before_handle(crow::request& req, crow::response& res, context& ctx)
    {
        int owner_check = check_owner_credentials(req);
        if (owner_check > 200)
        {
            res.code = owner_check;
            res.end();
        }
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx)
    {}
};

#endif //MIDDLEWARES_AUTH_MIDDLEWARES_H