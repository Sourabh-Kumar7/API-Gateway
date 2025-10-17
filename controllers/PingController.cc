#include <drogon/HttpController.h>
#include "ratelimiter/RateLimiterManager.h"
#include <memory>
#include <mutex>

using namespace drogon;

class PingController : public drogon::HttpController<PingController>
{
public:
    METHOD_LIST_BEGIN
    // Responds to GET /ping
    ADD_METHOD_TO(PingController::getPing, "/ping", Get);
    METHOD_LIST_END

    void getPing(const HttpRequestPtr &req, std::function<void (const HttpResponsePtr &)> &&callback)
    {
        (void)req;

        // rate limit: 5 calls per minute for the /ping endpoint (global)
        static std::shared_ptr<ratelimiter::RateLimiterManager> manager = std::make_shared<ratelimiter::RateLimiterManager>();
        static std::once_flag initFlag;
        std::call_once(initFlag, [&]() {
            ratelimiter::Policy p;
            p.capacity = 5;
            p.refill_rate = 5.0 / 60.0; // tokens per second
            manager->configurePolicy("global:ping", p);
        });

        if (!manager->tryConsume("global", "ping", 1)) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k429TooManyRequests);
            resp->setContentTypeCode(CT_APPLICATION_JSON);
            resp->setBody("{\"error\": \"rate limit exceeded\"}");
            callback(resp);
            return;
        }

        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k200OK);
        resp->setContentTypeCode(CT_TEXT_PLAIN);
        resp->setBody("pong");
        callback(resp);
    }
};
