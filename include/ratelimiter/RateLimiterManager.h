#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <unordered_map>
#include <shared_mutex>
#include "ratelimiter/TokenBucket.h"
#include "ratelimiter/RedisBucketSync.h"

namespace ratelimiter {

struct Policy {
    uint64_t capacity;
    double refill_rate; // tokens per second
};

class RateLimiterManager {
public:
    explicit RateLimiterManager(std::shared_ptr<RedisBucketSync> backend = nullptr);

    // Try to consume tokens for a (userId, apiId) pair. Returns true if allowed.
    bool tryConsume(const std::string &userId, const std::string &apiId, uint64_t tokens = 1);

    // Configure or update a policy for an api or user
    void configurePolicy(const std::string &key, const Policy &p);

private:
    // Build a lookup key for per-user+api buckets
    static std::string makeKey(const std::string &userId, const std::string &apiId);

    std::shared_ptr<RedisBucketSync> backend_;

    // In-memory buckets for single-process operation
    mutable std::shared_mutex mu_;
    std::unordered_map<std::string, TokenBucket> buckets_;
    std::unordered_map<std::string, Policy> policies_;
};

} // namespace ratelimiter
