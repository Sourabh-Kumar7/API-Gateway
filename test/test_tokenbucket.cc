#include <drogon/drogon_test.h>
#include <thread>
#include "ratelimiter/TokenBucket.h"

DROGON_TEST(TokenBucketBasic)
{
    using namespace ratelimiter;
    TokenBucket tb(10, 5.0); // capacity 10, refill 5 tokens/sec

    // Consume 10 tokens immediately
    for (int i = 0; i < 10; ++i)
    {
        ASSERT_TRUE(tb.tryConsume(1));
    }

    // Bucket should be empty now
    ASSERT_FALSE(tb.tryConsume(1));

    // Wait ~200ms -> 1 token should be refilled (5 tokens/sec)
    std::this_thread::sleep_for(std::chrono::milliseconds(220));
    ASSERT_TRUE(tb.tryConsume(1));

    // Ensure getTokens reports something reasonable
    double t = tb.getTokens();
    ASSERT_GE(t, 0.0);
    ASSERT_LE(t, 10.0);
}
