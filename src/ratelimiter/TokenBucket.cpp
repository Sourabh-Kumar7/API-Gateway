// TokenBucket.cpp
#include "ratelimiter/TokenBucket.h"

namespace ratelimiter {

TokenBucket::TokenBucket(uint64_t capacity, double refill_rate)
    : tokens_(static_cast<double>(capacity)), capacity_(capacity), refill_rate_(refill_rate), last_refill_(clock::now())
{
}

bool TokenBucket::tryConsume(uint64_t tokens)
{
    std::lock_guard<std::mutex> lk(mu_);
    refill();
    if (tokens_ + 1e-9 >= static_cast<double>(tokens)) {
        tokens_ -= static_cast<double>(tokens);
        return true;
    }
    return false;
}

double TokenBucket::getTokens() const
{
    std::lock_guard<std::mutex> lk(mu_);
    // call refill but const so emulate quick calculation
    auto now = clock::now();
    std::chrono::duration<double> elapsed = now - last_refill_;
    double added = elapsed.count() * refill_rate_;
    double t = tokens_ + added;
    if (t > static_cast<double>(capacity_)) t = static_cast<double>(capacity_);
    return t;
}

void TokenBucket::refill()
{
    auto now = clock::now();
    if (now <= last_refill_) return;
    std::chrono::duration<double> elapsed = now - last_refill_;
    double added = elapsed.count() * refill_rate_;
    if (added > 0.0) {
        tokens_ += added;
        if (tokens_ > static_cast<double>(capacity_)) tokens_ = static_cast<double>(capacity_);
        last_refill_ = now;
    }
}

} // namespace ratelimiter
