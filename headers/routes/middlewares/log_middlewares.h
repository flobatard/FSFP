#ifndef MIDDLEWARES_LOG_MIDDLEWARES_H
#define MIDDLEWARES_LOG_MIDDLEWARES_H

#define CROW_USE_BOOST 1

#include <boost/asio.hpp>
#include <crow.h>
#include <chrono>

struct TimeLogMiddleware
{
    struct context
    {
        std::chrono::high_resolution_clock::time_point beginningClock;
    };

    void before_handle(crow::request&, crow::response&, context& ctx)
    {
        ctx.beginningClock = std::chrono::high_resolution_clock::now();   
    }

    void after_handle(crow::request& req, crow::response&, context& ctx)
    {
        if (std::chrono::system_clock::to_time_t(ctx.beginningClock) == 0)
        {
            ctx.beginningClock =  std::chrono::high_resolution_clock::now();
        }
        std::chrono::high_resolution_clock::time_point endClock = std::chrono::high_resolution_clock::now();
        
        auto duration = duration_cast<std::chrono::nanoseconds>(endClock - ctx.beginningClock);
        unsigned int den = 1;
        std::string unit = "ns";
        if (duration <= std::chrono::microseconds(1))
        {
            den = 1;
        }
        else if (duration > std::chrono::microseconds(1) && duration < std::chrono::milliseconds(1))
        {
            den = 1000;
            unit = "μs";
        }
        else if (duration > std::chrono::milliseconds(1) && duration < std::chrono::seconds(10))
        {
            den = 1000000;
            unit = "ms";
        }
        else
        {

        }
        CROW_LOG_INFO << "Response: " << req.url << " : Response time : " << duration.count() / den << " " << unit;
    }
};

#endif
