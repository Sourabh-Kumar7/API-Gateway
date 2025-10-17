#include "ratelimiter/RateLimiterManager.h"

namespace ratelimiter {

RateLimiterManager::RateLimiterManager(std::shared_ptr<RedisBucketSync> backend)
    : backend_(backend)
{
}

std::string RateLimiterManager::makeKey(const std::string &userId, const std::string &apiId)
{
    return userId + ":" + apiId;
}

void RateLimiterManager::configurePolicy(const std::string &key, const Policy &p)
{
    std::unique_lock lock(mu_);
    policies_[key] = p;
    // If a bucket exists, update it by recreating
    auto it = buckets_.find(key);
    if (it != buckets_.end()) {
        // TokenBucket contains a mutex and is non-assignable/non-movable.
        // Erase and emplace a new TokenBucket constructed in-place.
        buckets_.erase(it);
        buckets_.emplace(std::piecewise_construct,
                         std::forward_as_tuple(key),
                         std::forward_as_tuple(p.capacity, p.refill_rate));
    }
    if (backend_) {
        backend_->ensureBucket(key, p.capacity, p.refill_rate);
    }
}

bool RateLimiterManager::tryConsume(const std::string &userId, const std::string &apiId, uint64_t tokens)
{
    std::string key = makeKey(userId, apiId);

    // If we have a distributed backend, attempt distributed consume first.
    if (backend_) {
        bool ok = backend_->tryConsume(key, tokens);
        if (ok) return true;
        // fallthrough to local attempt if backend indicates not allowed
    }

    // Local in-memory path
    {
        std::shared_lock lock(mu_);
        auto it = buckets_.find(key);
        if (it != buckets_.end()) {
            return it->second.tryConsume(tokens);
        }
    }

    // Create bucket lazily with policy if available
    std::unique_lock lock(mu_);
    auto pit = policies_.find(key);
    Policy p{10, 1.0};
    if (pit != policies_.end()) p = pit->second;
    auto [it, inserted] = buckets_.emplace(std::piecewise_construct,
                                           std::forward_as_tuple(key),
                                           std::forward_as_tuple(p.capacity, p.refill_rate));
    return it->second.tryConsume(tokens);
}

} // namespace ratelimiter
