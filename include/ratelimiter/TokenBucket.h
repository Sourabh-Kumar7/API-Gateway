// TokenBucket.h
#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>

namespace ratelimiter {

class TokenBucket {
public:
    using clock = std::chrono::steady_clock;

    // capacity: max tokens in bucket
    // refill_rate: tokens per second
    TokenBucket(uint64_t capacity, double refill_rate);

    // Try to consume `tokens`. Returns true if successful, false otherwise.
    bool tryConsume(uint64_t tokens = 1);

    // Returns current token count (approximate)
    double getTokens() const;

    // Refill tokens according to elapsed time (can be called periodically)
    void refill();

private:
    mutable std::mutex mu_;
    double tokens_;
    uint64_t capacity_;
    double refill_rate_; // tokens per second
    clock::time_point last_refill_;
};

} // namespace ratelimiter
